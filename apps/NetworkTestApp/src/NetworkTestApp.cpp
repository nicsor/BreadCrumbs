#include <NetworkTestApp.hpp>

#include <core/util/EnumCast.hpp>
#include <core/logger/event_logger.h>
#include <thread>

#include <iostream>

namespace
{
    const char *DOMAIN = "NetworkTestApp";

    core::util::EnumCast<bool> runAsMode =
        core::util::EnumCast<bool>
            (false, "client")
            (true, "server");
}

NetworkTestApp NetworkTestApp::_prototype;

NetworkTestApp::NetworkTestApp() :
    m_isActive(false),
    m_isServer(true),
    m_updatePeriodMs(2000),
    m_refreshSrvListPeriodMs(10000)
{
    addPrototype(DOMAIN, this);
}

NetworkTestApp::NetworkTestApp(const NetworkTestApp &other) :
    m_isActive(false),
    m_isServer(true),
    m_updatePeriodMs(2000),
    m_refreshSrvListPeriodMs(10000)
{
}

NetworkTestApp *NetworkTestApp::clone() const
{
    return new NetworkTestApp(*this);
}

void NetworkTestApp::init(const boost::property_tree::ptree::value_type &component)
{
    auto &conf = component.second;

    m_isServer = runAsMode(conf.get<std::string>("run-as", "server"));
    m_updatePeriodMs = conf.get<uint32_t>("update-period-ms", m_updatePeriodMs);
    m_refreshSrvListPeriodMs = conf.get<uint32_t>("refresh-server-list-ms", m_refreshSrvListPeriodMs);

    subscribe("NETWORK_DATA", std::bind(&NetworkTestApp::handleIncomingData, this, std::placeholders::_1));
    subscribe("UPDATE_SERVER_LIST", std::bind(&NetworkTestApp::handleServerListUpdate, this, std::placeholders::_1));
}

void NetworkTestApp::start()
{
    if (not m_isActive)
    {
        m_isActive = true;

        core::MessageData none;
        post("REFRESH_SERVER_LIST", none);

        if (not m_isServer)
        {
            m_refreshServerListTimer = setPeriodicTimer(
                [this](const boost::system::error_code &e)
                {
                    core::MessageData none;
                    post("REFRESH_SERVER_LIST", none);
                },
                std::chrono::milliseconds{m_refreshSrvListPeriodMs}
            );
        }

        m_broadcastMessageTimer = setPeriodicTimer(
            [this](const boost::system::error_code &e)
            {
                static int messaceCount = 0;
                std::string message = "Message " + std::to_string(++messaceCount);

                LOG_INFO(DOMAIN, " .... sending message[%s]: Data[%s]",
                    runAsMode(m_isServer).c_str(), message.c_str());

                core::MessageData attrs;
                attrs.set<std::string>("data", message);
                attrs.set<std::string>("id", runAsMode(m_isServer));
                post("NETWORK_BROADCAST", attrs);
            },
            std::chrono::milliseconds{m_updatePeriodMs}
        );
    }
}

void NetworkTestApp::stop()
{
    cancelTimer(m_broadcastMessageTimer);

    if (not m_isServer)
    {
        cancelTimer(m_refreshServerListTimer);
    }

    if (m_runner.joinable())
    {
        m_runner.join();
    }
}

void NetworkTestApp::handleIncomingData(const core::MessageData &attrs)
{
    LOG_INFO(DOMAIN, "NETWORK_DATA: Id[%s] Data[%s]",
        attrs.get<std::string>("id").c_str(),
        attrs.get<std::string>("data").c_str()
    );
}

void NetworkTestApp::handleServerListUpdate(const core::util::Attributes &attrs)
{
    std::stringstream sstr;
    uint32_t updateId = 0;

    sstr << "#############################" << std::endl;
    sstr << "UPDATE_SERVER_LIST:" << std::endl;

    for (auto it : attrs)
    {
        if (it.first != "update_id")
        {
            std::string serverAddress = attrs.get<std::string>(it.first).c_str();
            sstr << "[" << it.first.c_str() << "] => " << serverAddress.c_str() << std::endl;
        }
        else
        {
            updateId = attrs.get<uint32_t>(it.first);
            sstr << "[" << it.first.c_str() << "] => " << updateId << std::endl;
        }
    }
    sstr << "#############################" << std::endl;
    sstr << "Trigger connect to the first server" << std::endl;
    LOG_INFO(DOMAIN, "%s", sstr.str().c_str());

    core::MessageData connectDetails;
    connectDetails.set<int>("update_id", updateId);
    connectDetails.set<int>("id", 1);
    post("CONNECT_TO_SERVER", connectDetails);
}