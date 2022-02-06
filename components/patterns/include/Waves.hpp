#ifndef _COMPONENTS_PATTERNS_WAVES_
#define _COMPONENTS_PATTERNS_WAVES_

#include <core/Component.hpp>
#include <util/math/Cube.hpp>
#include <util/graphics/Color.hpp>

class Waves : public core::Component
{
private:
    Waves();

public:
    Waves *clone() const;
    void init(const boost::property_tree::ptree::value_type& component);

    void start();
    void stop();

protected:
    void waves(const boost::system::error_code &e);

private:
    uint32_t m_updatePeriodMs;
    bool m_enabled;
    core::TimerId m_timerId;
    util::math::Cube<util::graphics::Color> m_cubeData;

    static Waves _prototype;
};
#endif /* _COMPONENTS_PATTERNS_WAVES_ */
