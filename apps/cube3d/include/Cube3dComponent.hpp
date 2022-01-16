#ifndef _APPS_CUBE3D_COMPONENT_
#define _APPS_CUBE3D_COMPONENT_

#include "AppSettings.hpp"

#include <boost/property_tree/ptree.hpp>

#include <util/math/Cube.hpp>
#include <util/graphics/Color.hpp>

#include <core/Component.hpp>

class Cube3dComponent : public core::Component
{

private:
    Cube3dComponent();
	Cube3dComponent(const Cube3dComponent &base);

public:
    virtual ~Cube3dComponent();

	virtual Cube3dComponent* clone() const;
    void init(const boost::property_tree::ptree::value_type& component);
    
    void start();
    void stop();
    void clean();

protected:
    // Subscriptions
    void updateLed(const core::util::Attributes &attrs);
    void updateCube(const core::util::Attributes &attrs);
    void publishLedState(uint32_t x, uint32_t y, uint32_t z, const util::graphics::Color &color);

    // Periodic tasks
    void activeItemBlink(const boost::system::error_code &e);

    // Functional
    bool glInitialize(util::math::Cube<util::graphics::Color>& data);
    bool glExecute(util::math::Cube<util::graphics::Color>& data);
    void drawLine(const GLfloat *point1, const GLfloat *point2, const GLfloat *color);
    void drawCube(const AppSettings &instance);
    void printCurrentStatus();

private:
    int m_timerId;
    std::thread m_uiThread;
    util::math::Cube<util::graphics::Color> m_cubeData;
    static Cube3dComponent _prototype;
};

#endif /* _APPS_CUBE3D_COMPONENT_ */