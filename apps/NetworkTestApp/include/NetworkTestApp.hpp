#ifndef AURORA_CUBE_NETWORK_TEST_H_
#define AURORA_CUBE_NETWORK_TEST_H_

#include <core/Component.hpp>

#include <thread>

class NetworkTestApp : public core::Component
{
private:
    NetworkTestApp();
    NetworkTestApp(const NetworkTestApp &other);
    
public:
    NetworkTestApp *clone() const override;
    void init(const boost::property_tree::ptree::value_type &conf) override;

    void start() override;
    void stop() override;

protected:
    void handleIncomingData(const core::MessageId &id, const core::MessageData &attrs);
    void handleServerListUpdate(const core::MessageData &attrs);

private:
    static NetworkTestApp _prototype;

    bool m_isActive;
    bool m_isServer;
    uint32_t m_updatePeriodMs;
    uint32_t m_refreshSrvListPeriodMs;    
    std::thread m_runner;

    core::TimerId m_refreshServerListTimer;
    core::TimerId m_broadcastMessageTimer;
};

#endif