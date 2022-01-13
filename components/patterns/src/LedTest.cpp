#include "LedTest.hpp"

#include <core/logger/event_logger.h>

namespace
{
    const char *DOMAIN = "ledTest";

    const util::graphics::Color WHITE(0xFFFFFFFF);
    const util::graphics::Color BLACK(0);
}

LedTest LedTest::_prototype;

LedTest::LedTest() :
    m_cubeData(8),
    m_updatePeriodMs(100),
    m_updateRatePercentage(25),
    m_activeX(0),
    m_activeY(0),
    m_activeZ(0)
{
    addPrototype(DOMAIN, this);
}

LedTest *LedTest::clone() const
{
    return new LedTest(*this);
}

void LedTest::init(const boost::property_tree::ptree::value_type& component)
{
    auto config = component.second;

    uint32_t cubeSize = m_cubeData.size();

    cubeSize = config.get<uint32_t>("cube-size", cubeSize);
    m_updatePeriodMs = config.get<uint32_t>("update-rate-ms", m_updatePeriodMs);
    m_updateRatePercentage = config.get<uint8_t>("snow-rate-percent", m_updateRatePercentage);

    if (m_cubeData.size() != cubeSize)
    {
        m_cubeData.resize(cubeSize);
    }
}

void LedTest::stop()
{
    cancelTimer(m_timerId);
}

void LedTest::start()
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

    m_timerId = setPeriodicTimer(
        std::bind(&LedTest::ledTest, this, std::placeholders::_1),
        std::chrono::milliseconds(100));
}

void LedTest::ledTest(const boost::system::error_code &e)
{
    uint8_t cubeSize = m_cubeData.size();
    core::util::Attributes attr;
    attr.set("x", cubeSize);
    attr.set("y", cubeSize);
    attr.set("z", cubeSize);

    // Clear precious color
    m_cubeData(m_activeX, m_activeY, m_activeZ) = BLACK;

    // Update the active index
    ++m_activeX;
    if (m_activeX == cubeSize)
    {
        m_activeX = 0;
        ++m_activeY;
    }

    if (m_activeY == cubeSize)
    {
        m_activeY = 0;
        ++m_activeZ;
    }
 
    if (m_activeZ == cubeSize)
    {
        m_activeZ = 0;
    }

    // Update the active led
    m_cubeData(m_activeX, m_activeY, m_activeZ) = WHITE;

    attr.set("inputData", m_cubeData.toString());

    post("set_leds", attr);
}