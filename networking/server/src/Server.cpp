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

        subscribe("NETWORK_BROADCAST", std::bind(&Server::sendBroadcast, this, std::placeholders::_1));
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
        m_broadcastThread = std::thread([this]
                                        { this->m_ioContext->run(); });

        LOG_DEBUG(DOMAIN, "Exited %s", __PRETTY_FUNCTION__);
    }

    void Server::stop()
    {
        LOG_DEBUG(DOMAIN, "Entered %s", __PRETTY_FUNCTION__);
        m_isActive = false;
        m_acceptor->close();
        m_workGuard.reset();

        if (m_broadcastThread.joinable())
        {
            m_broadcastThread.join();
        }

        if (m_advertisingThread.joinable())
        {
            m_advertisingThread.join();
        }
        LOG_DEBUG(DOMAIN, "Exited %s", __PRETTY_FUNCTION__);
    }

    void Server::sendBroadcast(const core::MessageData &attrs)
    {
        LOG_DEBUG(DOMAIN, "Entered %s", __PRETTY_FUNCTION__);
        try
        {
            util::network::Message message(attrs.get<std::string>("id"), attrs.get<std::string>("data"));
            std::shared_ptr<std::string> buf = std::make_shared<std::string>(message.toString());
            for (auto sock : m_connectedClients)
            {
                if (sock->is_open())
                {
                    auto broadcast_handler = [this, sock, buf](const boost::system::error_code &ec, size_t bytes_sent)
                    {
                        if (ec)
                        {
                            LOG_ERROR(DOMAIN, "Error on broadcast.");
                        }
                    };
                    sock->async_send(boost::asio::buffer(*buf), broadcast_handler);
                }
            }
        }
        catch (std::exception &e)
        {
            LOG_ERROR(DOMAIN, "Error on broadcast: %s", e.what());
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
                    LOG_DEBUG(DOMAIN, "Received[%s] request from %s.",
                              (*buf).c_str(),
                              multicast_client_endpoint.address().to_string().c_str());

                    if ((*buf).find("ping") == 0)
                    {
                        const char *message = "pong";
                        m_multicastSocket->async_send_to(
                            boost::asio::buffer(message, sizeof(message)),
                            this->multicast_client_endpoint, [](const boost::system::error_code, size_t len) {});
                    }

                    // Continue to advertise for other potential clients.
                    advertise();
                }
                else
                {
                    LOG_ERROR(DOMAIN, "Error on advertise. Stopping ...");
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
            {
                std::scoped_lock<std::mutex> lock(*m_clientsMutex);
                m_connectedClients.push_back(sock);
            }
            m_acceptor->async_accept(
                *sock,
                [this, sock](const boost::system::error_code &ec)
                {
                    LOG_DEBUG(DOMAIN, "Entered %s", __PRETTY_FUNCTION__);
                    if (ec or not m_isActive)
                    {
                        LOG_ERROR(DOMAIN, "Error accepting data socket: %d \"%s\"", ec.value(), ec.message());
                    }
                    else
                    {
                        startListeningTcp();
                        sock->set_option(boost::asio::ip::tcp::no_delay(true));
                        LOG_DEBUG(DOMAIN, "Accepting data socket %p", sock.get());
                        try
                        {
                            receiveData(sock);
                        }
                        catch (...)
                        {
                        }
                        disconnect(sock);
                    }
                }
            );
        }
        LOG_DEBUG(DOMAIN, "Exited %s", __PRETTY_FUNCTION__);
    }

    void Server::receiveData(std::shared_ptr<boost::asio::ip::tcp::socket> sock, size_t bytes, void *buf)
    {
        LOG_DEBUG(DOMAIN, "Entered %s", __PRETTY_FUNCTION__);
        std::string data;
        data.resize(m_serverMaxRxMsgSizeKb * 1024);

        auto buffer = boost::asio::buffer(data);

        while (sock->is_open() && m_isActive)
        {
            size_t readSize = sock->read_some(buffer);
            LOG_DEBUG(DOMAIN, "Read %d bytes", readSize);

            util::network::Message message;
            message.fromString(std::string(&data[0], readSize));

            if (not message.isValid())
            {
                LOG_ERROR(DOMAIN, "Invalid message received. Closing connection.");
                return;
            }

            {
                core::MessageData attrs;
                attrs.set<std::string>("id", message.getId());
                attrs.set<std::string>("data", message.getData());
                LOG_DEBUG(DOMAIN, "Id[%s] Data[%s]", message.getId().c_str(), message.getData().c_str());
                post("NETWORK_DATA", attrs);
            }
        }

        LOG_DEBUG(DOMAIN, "Exited %s", __PRETTY_FUNCTION__);

        disconnect(sock);
        LOG_DEBUG(DOMAIN, "Exited %s", __PRETTY_FUNCTION__);
    }

    void Server::disconnect(std::shared_ptr<boost::asio::ip::tcp::socket> sock)
    {
        LOG_DEBUG(DOMAIN, "Entered %s", __PRETTY_FUNCTION__);
        std::scoped_lock lock(*(this->m_clientsMutex));
        this->m_connectedClients.erase(
            std::remove(
                this->m_connectedClients.begin(),
                this->m_connectedClients.end(),
                sock),
            this->m_connectedClients.end());
        LOG_DEBUG(DOMAIN, "Exited %s", __PRETTY_FUNCTION__);
    }
}