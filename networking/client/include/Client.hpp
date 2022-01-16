/**
 * @file Client.hpp
 *
 * @brief Implementation of a network client used
 *        to dispatch messages between systems.
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#ifndef _CORE_NETWORKING_CLIENT_
#define _CORE_NETWORKING_CLIENT_

#include <cstdint>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <boost/asio.hpp>

#include <core/Component.hpp>

namespace networking
{
    class Client : public core::Component
    {
    private:
        Client();
        Client(const Client &other);

    public:
        Client *clone() const override;
        void init(const boost::property_tree::ptree::value_type &component) override;

        void start() override;
        void stop() override;

    protected:
        void connectToServer(core::MessageData attr);
        void refreshServerList();
        void sendBroadcast(const core::MessageId &id, const core::MessageData &attrs);

    protected:
        void disconnect();
        void getServerList();
        void receiveData(std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    private:
        static Client _prototype;

        std::thread m_refreshServerListThread;
        std::thread m_broadcastThread;
        std::thread m_recvData;

        std::shared_ptr<boost::asio::io_context> m_ioContext;
        std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> m_workGuard;

        std::vector<std::string> m_detectedServers;

        std::string m_multicastAddress;
        uint16_t m_multicastPort;

        uint32_t m_serverRefreshTimeoutMs;
        uint32_t m_serverMaxRxMsgSizeKb;
        uint16_t m_serverTcpPort;
        std::shared_ptr<boost::asio::ip::tcp::socket> m_serverSocket;
        uint8_t m_maxNbrOfServers;

        uint32_t m_updateId;
        bool m_isActive;
    };
}

#endif /* _CORE_NETWORKING_CLIENT_ */
