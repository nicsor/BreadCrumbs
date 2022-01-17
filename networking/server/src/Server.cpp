/**
 * @file Server.cpp
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#include <Server.hpp>

#include <core/logger/event_logger.h>
#include <util/network/Message.hpp>

namespace networking
{
    namespace
    {
        const char *DOMAIN = "Server";
    }

    Server Server::_prototype;

    Server::Server() :
        m_multicastPort(5000),
        m_serverListenAddress("0.0.0.0"),
        m_serverListenPort(3000),
        m_isActive(true),
        m_serverMaxRxMsgSizeKb(16)
    {
        addPrototype(DOMAIN, this);
    }

    Server::Server(const Server &other)
    {
        m_multicastPort        = other.m_multicastPort;
        m_serverListenAddress  = other.m_serverListenAddress;
        m_serverListenPort     = other.m_serverListenPort;
        m_isActive             = other.m_isActive;
        m_serverMaxRxMsgSizeKb = other.m_serverMaxRxMsgSizeKb;
    }

    Server *Server::clone() const
    {
        return new Server(*this);
    }

    void Server::init(const boost::property_tree::ptree::value_type &component)
    {
        LOG_DEBUG(DOMAIN, "Entered %s", __PRETTY_FUNCTION__);
        auto &conf = component.second;
        m_multicastAddress = conf.get<std::string>("multicast.group");
        m_multicastPort = conf.get<uint16_t>("multicast.port", m_multicastPort);
        m_serverListenAddress = conf.get<std::string>("server.tcp.address", m_serverListenAddress);
        m_serverListenPort = conf.get<uint16_t>("server.tcp.port", m_serverListenPort);
        m_serverMaxRxMsgSizeKb = conf.get<uint32_t>("server.max.rx-size-kb", m_serverMaxRxMsgSizeKb);

        // Broascast message
        boost::property_tree::ptree defaultSubscribe;
        boost::property_tree::ptree networkData;
        networkData.put("","NETWORK_BROADCAST");
        defaultSubscribe.push_back(std::make_pair("", networkData));
        for (auto &subscription : conf.get_child("subscribe", defaultSubscribe))
        {
            std::string msgName = subscription.second.get<std::string>("");
            LOG_ERROR(DOMAIN, "Subscribing to [%s] to handle network broadcasts", msgName.c_str());
            subscribe(msgName, std::bind(&Server::sendBroadcast, this, msgName, std::placeholders::_1));
        }
    }

    void Server::start()
    {
        LOG_DEBUG(DOMAIN, "Entered %s", __PRETTY_FUNCTION__);
        m_ioContext = std::make_shared<boost::asio::io_context>();
        m_acceptor = std::make_shared<boost::asio::ip::tcp::acceptor>(
            *m_ioContext,
            boost::asio::ip::tcp::endpoint(boost::asio::ip::make_address(m_serverListenAddress), m_serverListenPort));
        m_clientsMutex = std::make_shared<std::mutex>();
        startListeningTcp();
        m_advertisingThread = std::thread(&Server::startAdvertising, this);

        m_workGuard = std::make_unique<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>(
            m_ioContext->get_executor());
        m_broadcastThread = std::thread([this]{ this->m_ioContext->run(); });

        LOG_DEBUG(DOMAIN, "Exited %s", __PRETTY_FUNCTION__);
    }

    void Server::stop()
    {
        LOG_DEBUG(DOMAIN, "Entered %s", __PRETTY_FUNCTION__);

        m_isActive = false;
        m_acceptor->close();
        m_workGuard.reset();

        if (m_multicastSocket->is_open())
        {
            LOG_DEBUG(DOMAIN, "Closing advertising socket");
            boost::system::error_code error;
            m_multicastSocket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
            m_multicastSocket->close();
        }

        if (m_broadcastThread.joinable())
        {
            LOG_DEBUG(DOMAIN, "Joining broadcast thread");
            m_broadcastThread.join();
        }

        if (m_advertisingThread.joinable())
        {
            LOG_DEBUG(DOMAIN, "Joining advertising thread");
            m_advertisingThread.join();
        }

        // Disconnecting any clients
        {
            LOG_DEBUG(DOMAIN, "Closing sockets");
            std::scoped_lock<std::mutex> lock(*m_clientsMutex);

            for (auto &sock : m_connectedClients)
            {
                if (sock->is_open())
                {
                    boost::system::error_code error;
                    sock->shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
                    sock->close();
                }
            }

            m_connectedClients.clear();
        }

        LOG_DEBUG(DOMAIN, "Joining all client threads");
        for (auto &runner: m_runners)
        {
            if (runner.joinable())
            {
                runner.join();
            }
        }

        LOG_DEBUG(DOMAIN, "Exited %s", __PRETTY_FUNCTION__);
    }

    void Server::sendBroadcast(const core::MessageId &id, const core::MessageData &attrs)
    {
        LOG_DEBUG(DOMAIN, "Entered %s", __PRETTY_FUNCTION__);

        util::network::Message message(id, attrs.get<std::string>("data"));
        std::shared_ptr<std::string> buf = std::make_shared<std::string>(message.toString());

        std::scoped_lock<std::mutex> lock(*m_clientsMutex);

        for (auto sock : m_connectedClients)
        {
            if (sock->is_open())
            {
                auto broadcast_handler = [this, sock, buf](const boost::system::error_code &ec, size_t bytes_sent)
                {
                    if (ec)
                    {
                        //std::string clientIp = sock.get()->remote_endpoint().address().to_string().c_str();
                        //LOG_ERROR(DOMAIN, "Error[%c] while broadcasting message to %s", clientIp.c_str());
                        LOG_ERROR(DOMAIN, "Error while broadcasting message to %p", sock.get());
                    }
                };
                sock->async_send(boost::asio::buffer(*buf), broadcast_handler);
            }
        }

        LOG_DEBUG(DOMAIN, "Exited %s", __PRETTY_FUNCTION__);
    }

    void Server::startAdvertising()
    {
        LOG_DEBUG(DOMAIN, "Entered %s", __PRETTY_FUNCTION__);

        boost::asio::io_context io;
        m_multicastSocket = std::make_shared<boost::asio::ip::udp::socket>(io);
        boost::asio::ip::udp::endpoint multicastEndpoint = boost::asio::ip::udp::endpoint(
            boost::asio::ip::make_address(m_serverListenAddress),
            m_multicastPort);
        m_multicastSocket->open(multicastEndpoint.protocol());
        m_multicastSocket->bind(multicastEndpoint);
        m_multicastSocket->set_option(boost::asio::ip::udp::socket::reuse_address(true));
        m_multicastSocket->set_option(
            boost::asio::ip::multicast::join_group(boost::asio::ip::make_address(m_multicastAddress)));
        advertise();
        io.run();

        LOG_DEBUG(DOMAIN, "Exited %s", __PRETTY_FUNCTION__);
    }

    void Server::advertise()
    {
        LOG_DEBUG(DOMAIN, "Entered %s", __PRETTY_FUNCTION__);

        std::shared_ptr<std::string> buf = std::make_shared<std::string>("pingpong");
        m_multicastSocket->async_receive_from(
            boost::asio::buffer(*buf),
            multicast_client_endpoint,
            [this, buf](const boost::system::error_code &ec, size_t length)
            {
                if (not ec)
                {
                    // Continue to advertise for other potential clients.
                    advertise();

                    LOG_DEBUG(DOMAIN, "Received[%s] request from %s.",
                              (*buf).c_str(),
                              multicast_client_endpoint.address().to_string().c_str());

                    if ((*buf).find("ping") == 0)
                    {
                        const char *message = "pong";
                        boost::system::error_code error;
                        uint32_t sendBytes = m_multicastSocket->send_to(
                            boost::asio::buffer(message, strlen(message)),
                            this->multicast_client_endpoint,
                            boost::asio::socket_base::message_flags(0),
                            error);

                        if (error)
                        {
                            LOG_ERROR(DOMAIN, "Error code[%d] while sending [%s]",
                                error,
                                message);
                        }
                        else
                        {
                            LOG_DEBUG(DOMAIN, "Sent back [%s] to [%s].",
                                message,
                                multicast_client_endpoint.address().to_string().c_str());
                        }
                    }
                }
                else
                {
                    if (m_isActive)
                    {
                        LOG_ERROR(DOMAIN, "Error detected while advertising");
                    }
                    else
                    {
                        LOG_INFO(DOMAIN, "Stopped advertising");
                    }
                }
            });

        LOG_DEBUG(DOMAIN, "Exited %s", __PRETTY_FUNCTION__);
    }

    void Server::startListeningTcp()
    {
        LOG_DEBUG(DOMAIN, "Entered %s", __PRETTY_FUNCTION__);

        if (m_isActive)
        {
            std::shared_ptr<boost::asio::ip::tcp::socket> sock = std::make_shared<boost::asio::ip::tcp::socket>(*m_ioContext);

            m_acceptor->async_accept(
                *sock,
                [this, sock](const boost::system::error_code &ec)
                {
                    if (ec or not m_isActive)
                    {
                        LOG_ERROR(DOMAIN, "Error[%d] while accepting data socket: [%s]",
                            ec.value(),
                            ec.message().c_str());
                    }
                    else
                    {
                        startListeningTcp();

                        sock->set_option(boost::asio::ip::tcp::no_delay(true));
                        m_runners.push_back(std::move(std::thread([this, sock]{ receiveData(sock);})));
                    }
                }
            );
        }

        LOG_DEBUG(DOMAIN, "Exited %s", __PRETTY_FUNCTION__);
    }

    void Server::receiveData(std::shared_ptr<boost::asio::ip::tcp::socket> sock)
    {
        LOG_DEBUG(DOMAIN, "Entered %s", __PRETTY_FUNCTION__);

        std::string clientIp = sock.get()->remote_endpoint().address().to_string().c_str();

        // Keep a reference to this socket
        {
            std::scoped_lock<std::mutex> lock(*m_clientsMutex);
            LOG_INFO(DOMAIN, "Client[%s]: connected - %p", clientIp.c_str(), sock.get());
            m_connectedClients.insert(sock);
        }

        std::string data;
        data.resize(m_serverMaxRxMsgSizeKb * 1024);

        auto buffer = boost::asio::buffer(data);

        while (sock->is_open() && m_isActive)
        {
            boost::system::error_code error;
            size_t readSize = sock->read_some(buffer, error);

            if (readSize != 0)
            {
                LOG_DEBUG(DOMAIN, "Client[%s]: Received %d bytes", clientIp.c_str(), readSize);
            }
            else if (error == boost::asio::error::eof)
            {
                continue;
            }
            else
            {
                LOG_ERROR(DOMAIN, "Client[%s]: Error on read: [%d]", clientIp.c_str(), error);
                break;
            }

            util::network::Message message;
            try
            {
                message.fromString(std::string(&data[0], readSize));
            }
            catch (std::exception e)
            {
                LOG_ERROR(DOMAIN, "Client[%s]: Unable to deserialize message received. Skiping[%s]",
                    clientIp.c_str(),
                    e.what());
                continue;
            }

            if (not message.isValid())
            {
                LOG_ERROR(DOMAIN, "Client[%s]: Invalid message received. Skiping.",
                    clientIp.c_str());
                continue;
            }

            {
                core::MessageData attrs;
                attrs.set<std::string>("data", message.getData());
                LOG_DEBUG(DOMAIN, "Client[%s]: Received message with id [%s]",
                    clientIp.c_str(),
                    message.getId().c_str());
                post(message.getId(), attrs);
            }
        }

        // Remove the reference to this connection
        {
            LOG_INFO(DOMAIN, "Client[%s]: Disconecting", clientIp.c_str());
            std::scoped_lock<std::mutex> lock(*m_clientsMutex);
            m_connectedClients.erase(sock);

            if (sock->is_open())
            {
                boost::system::error_code error;
                sock->shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
                sock->close();
            }
        }

        LOG_DEBUG(DOMAIN, "Exited %s", __PRETTY_FUNCTION__);
    }
}