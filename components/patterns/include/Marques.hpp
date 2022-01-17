#ifndef _COMPONENTS_PATTERNS_MARQUES_
#define _COMPONENTS_PATTERNS_MARQUES_

#include <core/Component.hpp>
#include <util/math/Cube.hpp>
#include <util/graphics/Color.hpp>

class Marques : public core::Component
{
private:
    Marques();

public:
    Marques *clone() const;
    void init(const boost::property_tree::ptree::value_type& component);

    void start();
    void stop();

protected:
    void update(const boost::system::error_code &e);
    void clearCube();

private:
    std::string m_text;
    uint32_t m_currentIndex;
    uint32_t m_currentIteration;
    uint32_t m_updatePeriodMs;
    bool m_enabled;
    core::TimerId m_timerId;
    util::math::Cube<util::graphics::Color> m_cubeData;

    static Marques _prototype;
};
#endif /* _COMPONENTS_PATTERNS_MARQUES_ */