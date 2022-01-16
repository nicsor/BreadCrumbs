#include "BoxIt.hpp"

#include <core/logger/event_logger.h>

namespace
{
    const char *DOMAIN = "boxIt";

    const util::graphics::Color WHITE(0xFFFFFFFF);
    const util::graphics::Color BLACK(0);
}

BoxIt BoxIt::_prototype;

BoxIt::BoxIt() :
    m_updatePeriodMs(0),
    m_enabled(false),
    m_cubeData(8)
{
    addPrototype(DOMAIN, this);
}

BoxIt *BoxIt::clone() const
{
    return new BoxIt(*this);
}

void BoxIt::init(const boost::property_tree::ptree::value_type& component)
{
    auto config = component.second;

    uint32_t cubeSize = m_cubeData.size();

    cubeSize = config.get<uint32_t>("cube-size", cubeSize);
    m_updatePeriodMs = config.get<uint32_t>("update-rate-ms", m_updatePeriodMs);
    m_enabled = config.get<bool>("enabled", m_enabled);

    if (m_cubeData.size() != cubeSize)
    {
        m_cubeData.resize(cubeSize);
    }
}

void BoxIt::stop()
{
    cancelTimer(m_timerId);
}

void BoxIt::start()
{
    for (int i =0; i < m_cubeData.size(); ++i)
    {
        for(uint8_t j = 0; j < m_cubeData.size(); ++j)
        {
            for(uint8_t l = 0; l < m_cubeData.size(); ++l)
            {
                m_cubeData(i, j, l) = BLACK;
            }
        }
    }

    size_t e = m_cubeData.size() - 1;

    for (int x = 0; x <= e; x++)
    {
        m_cubeData(x, 0, 0) = WHITE;
        m_cubeData(x, 0, e) = WHITE;
        m_cubeData(x, e, 0) = WHITE;
        m_cubeData(x, e, e) = WHITE;
        m_cubeData(0, x, 0) = WHITE;
        m_cubeData(0, x, e) = WHITE;
        m_cubeData(e, x, 0) = WHITE;
        m_cubeData(e, x, e) = WHITE;
        m_cubeData(0, 0, x) = WHITE;
        m_cubeData(0, e, x) = WHITE;
        m_cubeData(e, 0, x) = WHITE;
        m_cubeData(e, e, x) = WHITE;
    }

    if (m_enabled)
    {
        boost::system::error_code error;
        update(error);

        if (m_updatePeriodMs > 0)
        {
            m_timerId = setPeriodicTimer(
                std::bind(&BoxIt::update, this, std::placeholders::_1),
                std::chrono::milliseconds(m_updatePeriodMs));
        }
    }
}

void BoxIt::update(const boost::system::error_code &e)
{
    core::MessageData attr;
    attr.set("data", m_cubeData.toString());
    post("updateCube", attr);
}