#ifndef _COMPONENTS_PATTERNS_LEDTEST_
#define _COMPONENTS_PATTERNS_LEDTEST_

#include <core/Component.hpp>
#include <util/math/Cube.hpp>
#include <util/graphics/Color.hpp>

class LedTest : public core::Component
{
private:
    LedTest();

public:
    LedTest *clone() const;
    void init(const boost::property_tree::ptree::value_type& component);

    void start();
    void stop();

protected:
    void ledTest(const boost::system::error_code &e);

private:
    uint32_t m_updatePeriodMs;
    uint8_t m_updateRatePercentage;
    uint8_t m_activeX;
    uint8_t m_activeY;
    uint8_t m_activeZ;
    bool m_enabled;
    core::TimerId m_timerId;
    util::math::Cube<util::graphics::Color> m_cubeData;

    static LedTest _prototype;
};
#endif /* _COMPONENTS_PATTERNS_LEDTEST_ */