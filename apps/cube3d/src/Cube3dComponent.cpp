#include <Cube3dComponent.hpp>

#include <thread>

#include <iostream>
#include <iomanip>

#include <core/logger/event_logger.h>
#include <common/message/type/LedState.hpp>

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

    // Register callback for color update
    params.onColorUpdate = std::bind(
        &Cube3dComponent::publishLedState,
        this,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4);

    subscribe("updateLed", std::bind(&Cube3dComponent::updateLed, this, std::placeholders::_1));
    subscribe("updateCube", std::bind(&Cube3dComponent::updateCube, this, std::placeholders::_1));
    subscribe("cubeState", std::bind(&Cube3dComponent::updateCube, this, std::placeholders::_1));
}

void Cube3dComponent::updateLed(const core::util::Attributes &attrs)
{
    std::string data;
    attrs.get("data", data);

    common::message::type::LedState ledState;
    ledState.fromString(data);

    m_cubeData.set(ledState.x, ledState.y, ledState.z, ledState.color);
}

void Cube3dComponent::updateCube(const core::util::Attributes &attrs)
{
    std::string data;
    attrs.get("data", data);

    util::math::Cube<util::graphics::Color> cubeData(1);
    m_cubeData.fromString(data);
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

void Cube3dComponent::publishLedState(uint32_t x, uint32_t y, uint32_t z, const util::graphics::Color &color)
{
    core::MessageData attrs;

    common::message::type::LedState ledState;
    ledState.x = x;
    ledState.y = y;
    ledState.z = z;
    ledState.color = color;

    attrs.set("data", ledState.toString());
    post("updateLed", attrs);
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