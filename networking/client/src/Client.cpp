/**
 * @file Client.cpp
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#include <Client.hpp>

#include <core/logger/event_logger.h>
#include <util/network/Message.hpp>

namespace networking
{
    namespace
    {
        const char *DOMAIN = "Client";
    }

    Client Client::_prototype;

    Client::Client() :
        m_multicastPort(5000),
        m_serverTcpPort(3000),
        m_serverRefreshTimeoutMs(5000),
        m_serverMaxRxMsgSizeKb(16),
        m_maxNbrOfServers(2),
        m_updateId(0),
        m_isActive(true)
    {
        addPrototype("Client", this);
    }

    Client::Client(const Client &other)
    {
        m_multicastPort          = other.m_multicastPort;
        m_serverTcpPort          = other.m_serverTcpPort;
        m_serverRefreshTimeoutMs = other.m_serverRefreshTimeoutMs;
        m_serverMaxRxMsgSizeKb   = other.m_serverMaxRxMsgSizeKb;
        m_maxNbrOfServers        = other.m_maxNbrOfServers;
        m_updateId               = other.m_updateId;
        m_isActive               = other.m_isActive;
    }

    Client *Client::clone() const
    {
        return new Client(*this);
    }

    void Client::init(const boost::property_tree::ptree::value_type &component)
    {
        LOG_DEBUG(DOMAIN, "Entered %s", __PRETTY_FUNCTION__);

        auto &conf = component.second;

        m_multicastAddress = conf.get<std::string>("multicast.group"); // mandatory
        m_multicastPort = conf.get<uint16_t>("multicast.port", m_multicastPort);
        m_serverTcpPort = conf.get<uint16_t>("server.tcp.port", m_serverTcpPort);
        m_serverRefreshTimeoutMs = conf.get<uint32_t>("server.max.timeout-ms", m_serverRefreshTimeoutMs);
        m_serverMaxRxMsgSizeKb = conf.get<uint32_t>("server.max.rx-size-kb", m_serverMaxRxMsgSizeKb);
        m_maxNbrOfServers = conf.get<uint8_t>("server.max.count", m_maxNbrOfServers);

        subscribe("NETWORK_BROADCAST", std::bind(&Client::sendBroadcast, this, std::placeholders::_1));
        subscribe("CONNECT_TO_SERVER", std::bind(&Client::connectToServer, this, std::placeholders::_1));
        subscribe("REFRESH_SERVER_LIST", std::bind(&Client::refreshServerList, this));

        LOG_DEBUG(DOMAIN, "Exited %s", __PRETTY_FUNCTION__);
    }

    void Client::start()
    {
        LOG_DEBUG(DOMAIN, "Entered %s", __PRETTY_FUNCTION__);

        m_ioContext = std::make_shared<boost::asio::io_context>();

        m_workGuard = std::make_unique<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>(
            m_ioContext->get_executor());
        m_broadcastThread = std::thread(
            [this] { this->m_ioContext->run(); });

        refreshServerList();

        LOG_DEBUG(DOMAIN, "Exited %s", __PRETTY_FUNCTION__);
    }

    void Client::stop()
    {
        LOG_DEBUG(DOMAIN, "Entered %s", __PRETTY_FUNCTION__);

        m_isActive = false;

        disconnect();

        if (m_refreshServerListThread.joinable())
        {
            m_refreshServerListThread.join();
        }

        m_workGuard.reset();

        if (m_broadcastThread.joinable())
        {
            m_broadcastThread.join();
        }

        LOG_DEBUG(DOMAIN, "Exited %s", __PRETTY_FUNCTION__);
    }

    void Client::refreshServerList()
    {
        LOG_DEBUG(DOMAIN, "Entered %s", __PRETTY_FUNCTION__);

        if (m_isActive and not m_refreshServerListThread.joinable())
        {
            m_refreshServerListThread = std::thread(
                [&]
                {
                    getServerList();

                    // Idea is to make it so that the thread is not joinable anymore
                    // so that we don't run multiple refreshes at the same time
                    m_refreshServerListThread.detach();
                });
        }

        LOG_DEBUG(DOMAIN, "Exited %s", __PRETTY_FUNCTION__);
    }

    void Client::getServerList()
    {
        LOG_DEBUG(DOMAIN, "Entered %s", __PRETTY_FUNCTION__);

        boost::asio::io_context io_context;
        boost::asio::ip::udp::endpoint multicastEndpoint =
            boost::asio::ip::udp::endpoint(
                boost::asio::ip::address::from_string(m_multicastAddress), m_multicastPort);
        boost::asio::ip::udp::socket socket(io_context, multicastEndpoint.protocol());
        socket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
        socket.set_option(boost::asio::ip::multicast::join_group(
            boost::asio::ip::address::from_string(m_multicastAddress)));

        core::TimerId timer = setOneShotTimer(
            [this, &socket](const boost::system::error_code &ec)
            {
                LOG_DEBUG(DOMAIN, "Timeout on server refresh");
                if (socket.is_open())
                {
                    boost::system::error_code err;
                    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, err);
                }
            },
            std::chrono::milliseconds(m_serverRefreshTimeoutMs)
        );

        socket.send_to(boost::asio::buffer("ping"), multicastEndpoint);

        core::MessageData attr;
        attr.set<uint32_t>("update_id", ++m_updateId);
        m_detectedServers.clear();
        int serverCount = 0;

        while (serverCount < m_maxNbrOfServers)
        {
            char somedata[16];
            boost::asio::ip::udp::endpoint destinationEndpoint = multicastEndpoint;
            size_t bytes = socket.receive_from(
                boost::asio::buffer(somedata, sizeof(somedata)), destinationEndpoint);

            if (bytes && strncmp("pong", somedata, 4) == 0)
            {
                std::string ipAddress(destinationEndpoint.address().to_string());
                LOG_DEBUG(DOMAIN, "Received advertise from: [%s]", ipAddress.c_str());

                if (std::find(
                        this->m_detectedServers.begin(),
                        this->m_detectedServers.end(),
                        ipAddress) == m_detectedServers.end())
                {
                    attr.set<std::string>(std::to_string(serverCount + 1), ipAddress);
                    this->m_detectedServers.push_back(ipAddress);

                    post("UPDATE_SERVER_LIST", attr);
                    ++serverCount;
                }
            }
            else
            {
                // Probably timeout. exiting.
                break;
            }
        }

        cancelTimer(timer);
        if (socket.is_open())
        {
            socket.close();
        }

        LOG_DEBUG(DOMAIN, "Exited %s", __PRETTY_FUNCTION__);
    }

    void Client::sendBroadcast(const core::MessageData &attrs)
    {
        LOG_DEBUG(DOMAIN, "Entered %s", __PRETTY_FUNCTION__);
        if (not m_isActive or not m_serverSocket or not m_serverSocket->is_open())
        {
            LOG_DEBUG(DOMAIN, "Exited %sactive, [%p] %s",
                m_isActive ? "" : "not ",
                m_serverSocket.get(),
                __PRETTY_FUNCTION__);
            return;
        }

        util::network::Message message(
            attrs.get<std::string>("id"),
            attrs.get<std::string>("data"));
        std::shared_ptr<std::string> buf = std::make_shared<std::string>(message.toString());

        auto broadcast_handler = [this, buf](const boost::system::error_code &error, size_t bytes_sent)
        {
            if (error)
            {
                LOG_ERROR(DOMAIN, "Error[%d] on broadcast.", error.value());
            }
        };
        m_serverSocket->async_send(boost::asio::buffer(*buf), broadcast_handler);

        LOG_DEBUG(DOMAIN, "Exited %s", __PRETTY_FUNCTION__);
    }

    void Client::receiveData(std::shared_ptr<boost::asio::ip::tcp::socket> sock)
    {
        LOG_DEBUG(DOMAIN, "Entered %s", __PRETTY_FUNCTION__);

        std::string data;
        data.resize(m_serverMaxRxMsgSizeKb * 1024);
        std::string remoteIp = sock.get()->remote_endpoint().address().to_string().c_str();
        LOG_INFO(DOMAIN, "Remote[%s]: connected", remoteIp.c_str());

        auto buffer = boost::asio::buffer(data);

        while (sock->is_open() and m_isActive)
        {
            boost::system::error_code error;
            size_t readSize = sock->read_some(buffer, error);

            if (readSize != 0)
            {
                LOG_DEBUG(DOMAIN, "Remote[%s]: Received %d bytes", remoteIp.c_str(), readSize);
            }
            else if (error == boost::asio::error::eof)
            {
                continue;
            }
            else
            {
                LOG_ERROR(DOMAIN, "Remote[%s]: Error on read: [%d]", remoteIp.c_str(), error);
                break;
            }

            util::network::Message message;
            try
            {
                message.fromString(std::string(&data[0], readSize));
            }
            catch (std::exception e)
            {
                LOG_ERROR(DOMAIN, "Remote[%s]: Unable to deserialize message received. Disconnecting[%s]",
                    remoteIp.c_str(),
                    e.what());
                break;
            }

            if (not message.isValid())
            {
                LOG_ERROR(DOMAIN, "Remote[%s]: Invalid message received. Closing connection.",
                    remoteIp.c_str());
                return;
            }

            // Publish message
            {
                core::MessageData attrs;
                attrs.set<std::string>("id", message.getId());
                attrs.set<std::string>("data", message.getData());
                LOG_DEBUG(DOMAIN, "Remote[%s]: Received message with id [%s]",
                    remoteIp.c_str(),
                    message.getId().c_str());
                post("NETWORK_DATA", attrs);
            }
        }

        {
            LOG_INFO(DOMAIN, "Remote[%s]: Disconecting", remoteIp.c_str());
            disconnect();
        }

        LOG_DEBUG(DOMAIN, "Exited %s", __PRETTY_FUNCTION__);
    }

    void Client::connectToServer(core::MessageData attrs)
    {
        LOG_DEBUG(DOMAIN, "Entered %s", __PRETTY_FUNCTION__);
        int serverIndex = attrs.get<int>("id") - 1;

        if (serverIndex >= m_detectedServers.size())
        {
            LOG_DEBUG(DOMAIN, "Exited because of invalid server index => [%d] %s", serverIndex, __PRETTY_FUNCTION__);
            return;
        }

        int updateId = attrs.get<int>("update_id");

        boost::asio::ip::tcp::endpoint endpoint = boost::asio::ip::tcp::endpoint(
            boost::asio::ip::make_address(m_detectedServers.at(serverIndex)),
            m_serverTcpPort);

        disconnect(); // if connected

        LOG_DEBUG(DOMAIN, "Connecting to [%s]", endpoint.address().to_string().c_str());

        m_serverSocket = std::make_shared<boost::asio::ip::tcp::socket>(*m_ioContext);
        m_serverSocket->connect(endpoint);
        m_serverSocket->set_option(boost::asio::ip::tcp::no_delay(true));

        m_recvData = std::thread([this] { receiveData(m_serverSocket); });

        LOG_DEBUG(DOMAIN, "Exited %s", __PRETTY_FUNCTION__);
    }

    void Client::disconnect()
    {
        LOG_DEBUG(DOMAIN, "Entered %s", __PRETTY_FUNCTION__);

        if (m_serverSocket)
        {
            LOG_DEBUG(DOMAIN, "Socket %p is closing", m_serverSocket.get());
            // Force shutdown
            if (m_serverSocket->is_open())
            {
                boost::system::error_code err;
                m_serverSocket->shutdown(
                    boost::asio::ip::tcp::socket::shutdown_both, err);
                m_serverSocket->close();
            }
            m_serverSocket.reset();
        }

        if (m_recvData.joinable() and
            std::this_thread::get_id() != m_recvData.get_id())
        {
            m_recvData.join();
        }

        LOG_DEBUG(DOMAIN, "Exited %s", __PRETTY_FUNCTION__);
    }
}