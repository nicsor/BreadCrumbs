#include "Waves.hpp"

#include <random>

namespace
{
    std::random_device dev;
    std::mt19937 rng(dev());
    const std::vector<uint8_t> potentialColorValues = { 0x0, 0x55, 0xFF };
    std::uniform_int_distribution<std::mt19937::result_type> randomColor(0, potentialColorValues.size());
}

#include <core/logger/event_logger.h>

namespace
{
    const char *DOMAIN = "waves";
}

Waves Waves::_prototype;

Waves::Waves() :
    m_enabled(false),
    m_cubeData(8),
    m_updatePeriodMs(100)
{
    addPrototype(DOMAIN, this);
}

Waves *Waves::clone() const
{
    return new Waves(*this);
}

void Waves::init(const boost::property_tree::ptree::value_type& component)
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

void Waves::stop()
{
    cancelTimer(m_timerId);
}

void Waves::start()
{
    if (m_enabled)
    {
        m_timerId = setPeriodicTimer(
            std::bind(&Waves::waves, this, std::placeholders::_1),
            std::chrono::milliseconds(m_updatePeriodMs));
    }
}

void Waves::waves(const boost::system::error_code &e)
{
    uint8_t cubeSize = m_cubeData.size();

    core::MessageData attr;

    for (int i = 0; i < cubeSize; ++i)
    {
        for(uint8_t j = 0; j < cubeSize - 1;j++)
        {
            for(uint8_t l = 0; l < cubeSize;l++)
            {
                m_cubeData(i, j, l) = m_cubeData(i, j + 1, l);
            }
        }
    }

    util::graphics::Color color;
    color.rgb.R = potentialColorValues[randomColor(rng)];
    color.rgb.G = potentialColorValues[randomColor(rng)];
    color.rgb.B = potentialColorValues[randomColor(rng)];

    for (int i = 0; i < cubeSize; ++i)
    {
        for(uint8_t l = 1; l < cubeSize;l++)
        {
            m_cubeData(i, cubeSize - 1, l - 1) = m_cubeData(i, cubeSize - 1, l);
                
        }

        m_cubeData(i, cubeSize - 1, cubeSize - 1) = color;
    }

    attr.set("data", m_cubeData.toString());

    post("updateCube", attr);
}
