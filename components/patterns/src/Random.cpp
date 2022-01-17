#include "Random.hpp"

#include <random>

#include <core/logger/event_logger.h>

namespace
{
    const char *DOMAIN = "random";
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> randomColor(0, 3);
    const uint8_t values[5] = {0, /*0x1, 0x11,*/ 0x55, 0xFF};
}

Random Random::_prototype;

Random::Random() :
    m_updatePeriodMs(0),
    m_enabled(false),
    m_cubeData(8)
{
    addPrototype(DOMAIN, this);
}

Random *Random::clone() const
{
    return new Random(*this);
}

void Random::init(const boost::property_tree::ptree::value_type& component)
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

void Random::stop()
{
    cancelTimer(m_timerId);
}

void Random::start()
{
    LOG_ERROR(DOMAIN, "start random");
    if (m_enabled)
    {
        if (m_updatePeriodMs > 0)
        {
            m_timerId = setPeriodicTimer(
                std::bind(&Random::update, this, std::placeholders::_1),
                std::chrono::milliseconds(m_updatePeriodMs));
        }
    }
}

void Random::update(const boost::system::error_code &e)
{
    LOG_ERROR(DOMAIN, "update random");
    uint32_t cubeSize = m_cubeData.size();

    for (uint32_t i = 0; i < cubeSize; ++i)
    {
        for(uint32_t j = 0; j < cubeSize;j++)
        {
            for(uint32_t l = 0; l < cubeSize;l++)
            {
                m_cubeData(i, j, l).rgb.R = values[randomColor(rng)];
                m_cubeData(i, j, l).rgb.G = values[randomColor(rng)];
                m_cubeData(i, j, l).rgb.B = values[randomColor(rng)];
            }
        }
    }

    core::MessageData attr;
    attr.set("data", m_cubeData.toString());
    post("updateCube", attr);
}