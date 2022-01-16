#ifndef _COMPONENTS_PATTERNS_BOXIT_
#define _COMPONENTS_PATTERNS_BOXIT_

#include <core/Component.hpp>
#include <util/math/Cube.hpp>
#include <util/graphics/Color.hpp>

class BoxIt : public core::Component
{
private:
    BoxIt();

public:
    BoxIt *clone() const;
    void init(const boost::property_tree::ptree::value_type& component);

    void start();
    void stop();

protected:
    void update(const boost::system::error_code &e);

private:
    uint32_t m_updatePeriodMs;
    bool m_enabled;
    core::TimerId m_timerId;
    util::math::Cube<util::graphics::Color> m_cubeData;

    static BoxIt _prototype;
};
#endif /* _COMPONENTS_PATTERNS_BOXIT_ */