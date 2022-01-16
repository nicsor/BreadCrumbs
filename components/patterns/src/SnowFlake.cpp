#include "SnowFlake.hpp"

#include <random>

namespace
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> randomColor(0, 100);
}

#include <core/logger/event_logger.h>

namespace
{
    const char *DOMAIN = "snowFlake";
}

SnowFlake SnowFlake::_prototype;

SnowFlake::SnowFlake() :
    m_enabled(false),
    m_cubeData(8),
    m_updatePeriodMs(100),
    m_updateRatePercentage(25)
{
    addPrototype(DOMAIN, this);
}

SnowFlake *SnowFlake::clone() const
{
    return new SnowFlake(*this);
}

void SnowFlake::init(const boost::property_tree::ptree::value_type& component)
{
    auto config = component.second;

    uint32_t cubeSize = m_cubeData.size();

    cubeSize = config.get<uint32_t>("cube-size", cubeSize);
    m_updatePeriodMs = config.get<uint32_t>("update-rate-ms", m_updatePeriodMs);
    m_updateRatePercentage = config.get<uint8_t>("snow-rate-percent", m_updateRatePercentage);
    m_enabled = config.get<bool>("enabled", m_enabled);

    if (m_cubeData.size() != cubeSize)
    {
        m_cubeData.resize(cubeSize);
    }
}

void SnowFlake::stop()
{
    cancelTimer(m_timerId);
}

void SnowFlake::start()
{
    if (m_enabled)
    {
        m_timerId = setPeriodicTimer(
            std::bind(&SnowFlake::snowLeds, this, std::placeholders::_1),
            std::chrono::milliseconds(m_updatePeriodMs));
    }
}

void SnowFlake::snowLeds(const boost::system::error_code &e)
{
    static const util::graphics::Color WHITE(0xFFFFFFFF);
    static const util::graphics::Color BLACK(0);

    uint8_t cubeSize = m_cubeData.size();

    core::MessageData attr;

    for (int i =0; i < cubeSize; ++i)
    {
        for(uint8_t j = 0; j < cubeSize - 1;j++)
        {
            for(uint8_t l = 0; l < cubeSize;l++)
            {
                m_cubeData(i, j, l) = m_cubeData(i, j + 1, l);
            }
        }
    }

    for (int i =0; i < cubeSize; ++i)
    {
        for(uint8_t l = 0; l < cubeSize;l++)
        {
            m_cubeData(i, cubeSize - 1, l) = 
                (randomColor(rng) < m_updateRatePercentage) ? WHITE : BLACK;
        }
    }

    attr.set("data", m_cubeData.toString());

    post("updateCube", attr);
}