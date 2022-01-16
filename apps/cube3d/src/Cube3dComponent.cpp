#include <Cube3dComponent.hpp>

#include <thread>

#include <iostream>
#include <iomanip>

#include <core/logger/event_logger.h>

namespace
{
    const char *DOMAIN = "cube3D";
}

void closeApp(int code);

Cube3dComponent Cube3dComponent::_prototype;

Cube3dComponent::Cube3dComponent() :
    m_timerId(0),
    m_cubeData(8)
{
    addPrototype(DOMAIN, this);
}

Cube3dComponent::Cube3dComponent(const Cube3dComponent &base) :
    m_timerId(base.m_timerId),
    m_cubeData(base.m_cubeData)
{
}

Cube3dComponent::~Cube3dComponent()
{
    
}

Cube3dComponent* Cube3dComponent::clone() const
{
	return new Cube3dComponent(*this);
}

void Cube3dComponent::init(const boost::property_tree::ptree::value_type& component)
{
    AppSettings &params = AppSettings::getInstance();
    params.configure(component.second);

    subscribe("set_led_state", std::bind(&Cube3dComponent::updateLedState, this, std::placeholders::_1));
    subscribe("set_leds", std::bind(&Cube3dComponent::updateMatrix, this, std::placeholders::_1));
}

void Cube3dComponent::updateLedState(const core::util::Attributes &attrs)
{
    uint8_t x, y, z;
    uint32_t color;

    attrs.get("x", x);
    attrs.get("y", y);
    attrs.get("z", z);
    attrs.get("color", color);

    LOG_DEBUG(DOMAIN,"SET_LED_STATE[%d][%d][%d] = 0x%x ", (int)x,(int)y,(int)z, color);

    m_cubeData.set(x, y, z, util::graphics::Color(color));
}

void Cube3dComponent::updateMatrix(const core::util::Attributes &attrs)
{
    uint8_t sizeX, sizeY, sizeZ;
    std::string inputData;
    
    attrs.get("x", sizeX);
    attrs.get("y", sizeY);
    attrs.get("z", sizeZ);
    attrs.get("inputData", inputData);

    uint32_t offset = 0;

    m_cubeData.fromString(inputData);

    LOG_DEBUG(DOMAIN, "SET_LEDS: size[%d][%d][%d], data-len: [%d]",
             sizeX, sizeY, sizeZ, inputData.length());
}

void Cube3dComponent::activeItemBlink(const boost::system::error_code &e)
{
    AppSettings &instance = AppSettings::getInstance();

    if (
        instance.state.current.selectedLed.x != instance.state.previous.selectedLed.x ||
        instance.state.current.selectedLed.y != instance.state.previous.selectedLed.y ||
        instance.state.current.selectedLed.z != instance.state.previous.selectedLed.z)
    {
        m_cubeData.set(
            instance.state.current.selectedLed.x,
            instance.state.current.selectedLed.y,
            instance.state.current.selectedLed.z,
            instance.state.selectedColor.color);
        instance.state.selectedColor.color = m_cubeData(
            instance.state.previous.selectedLed.x,
            instance.state.previous.selectedLed.y,
            instance.state.previous.selectedLed.z).color;
        instance.state.current.selectedLed.x = instance.state.previous.selectedLed.x;
        instance.state.current.selectedLed.y = instance.state.previous.selectedLed.y;
        instance.state.current.selectedLed.z = instance.state.previous.selectedLed.z;
    }

    if (instance.state.blinkCounter == instance.config.blinkPeriod.on)
    {
        m_cubeData.set(
            instance.state.previous.selectedLed.x,
            instance.state.previous.selectedLed.y,
            instance.state.previous.selectedLed.z,
            0xFFFFFFFF - instance.state.selectedColor.color);
    }
    else if (instance.state.blinkCounter >= instance.config.blinkPeriod.on + instance.config.blinkPeriod.off)
    {
        m_cubeData.set(
            instance.state.previous.selectedLed.x,
            instance.state.previous.selectedLed.y,
            instance.state.previous.selectedLed.z,
            instance.state.selectedColor.color);
        instance.state.blinkCounter = 0;
    }
    ++instance.state.blinkCounter;
}

void Cube3dComponent::start()
{
    AppSettings &instance = AppSettings::getInstance();

    m_timerId = setPeriodicTimer(
        std::bind(&Cube3dComponent::activeItemBlink, this, std::placeholders::_1),
        std::chrono::milliseconds(instance.config.blinkPeriod.unitMs));

    m_uiThread = std::thread([&]
        {
            // Need to have these methods called from the same thread
            glInitialize(m_cubeData);
            while (not glExecute(m_cubeData))
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(instance.config.renderIntervalMs));
            }

            closeApp(0);
        });
}

void Cube3dComponent::stop()
{
    AppSettings &instance = AppSettings::getInstance();

    glfwSetWindowShouldClose(instance.state.window, GL_TRUE);
    if (m_uiThread.joinable())
    {
        m_uiThread.join();
    }
}

void Cube3dComponent::clean()
{
    cancelTimer(m_timerId);
}