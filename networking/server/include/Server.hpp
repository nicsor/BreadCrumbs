/**
 * @file Server.hpp
 *
 * @brief Implementation of a network server used
 *        to dispatch messages between systems.
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#ifndef _CORE_NETWORKING_SERVER_
#define _CORE_NETWORKING_SERVER_

#include <array>
#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>
#include <string>

#include <boost/asio.hpp>

#include <core/Component.hpp>

namespace networking
{

    class Server : public core::Component
    {
    private:
        Server();
        Server(const Server &other);

    public:
        Server *clone() const override;
        void init(const boost::property_tree::ptree::value_type &component) override;

        void start() override;
        void stop() override;

    protected:
        void sendBroadcast(const core::MessageData &attrs);

        void startAdvertising();
        void startListeningTcp();

        void advertise();
        void receiveData(std::shared_ptr<boost::asio::ip::tcp::socket> sock, size_t bytes = 0, void *buf = nullptr);
        void disconnect(std::shared_ptr<boost::asio::ip::tcp::socket> sock);

    private:
        static Server _prototype;

        std::shared_ptr<boost::asio::ip::udp::socket> m_multicastSocket;
        std::shared_ptr<boost::asio::ip::tcp::acceptor> m_acceptor;

        std::thread m_advertisingThread;
        std::thread m_broadcastThread;

        std::string m_multicastAddress;
        uint16_t m_multicastPort;
        std::string m_serverListenAddress;
        uint16_t m_serverListenPort;
        uint32_t m_serverMaxRxMsgSizeKb;

        std::shared_ptr<boost::asio::io_context> m_ioContext;
        std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> m_workGuard;

        std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>> m_connectedClients;
        std::shared_ptr<std::mutex> m_clientsMutex;
        bool m_isActive;

        boost::asio::ip::udp::endpoint multicast_client_endpoint;
    };
}

#endif /* _CORE_NETWORKING_SERVER_ */