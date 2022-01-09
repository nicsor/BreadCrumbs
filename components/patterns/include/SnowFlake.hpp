#ifndef _COMPONENTS_PATTERNS_SNOWFLAKE_
#define _COMPONENTS_PATTERNS_SNOWFLAKE_

#include <core/Component.hpp>
#include <util/math/Cube.hpp>
#include <util/graphics/Color.hpp>

class SnowFlake : public core::Component
{
private:
    SnowFlake();

public:
    SnowFlake *clone() const;
    void init(const boost::property_tree::ptree::value_type& component);

    void start();
    void stop();

protected:
    void snowLeds(const boost::system::error_code &e);

private:
    uint32_t m_updatePeriodMs;
    uint8_t m_updateRatePercentage;
    core::TimerId m_timerId;
    util::math::Cube<util::graphics::Color> m_cubeData;

    static SnowFlake _prototype;
};
#endif /* _COMPONENTS_PATTERNS_SNOWFLAKE_ */