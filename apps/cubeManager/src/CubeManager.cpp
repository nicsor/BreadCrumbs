/**
 * @file CubeManager.cpp
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#include "CubeManager.hpp"

#include <iomanip>

#include <core/logger/event_logger.h>
#include <common/message/type/LedState.hpp>

#include "AppSettings.hpp"

namespace {
    const char * DOMAIN = "cubeManager";
}

CubeManager CubeManager::_prototype;

CubeManager::CubeManager() :
    m_numLayers(8),
    m_activeLayer(0),
    m_activeDigit(0)
{
    addPrototype(DOMAIN, this);
}

CubeManager *CubeManager::clone() const
{
    return new CubeManager(*this);
}

void CubeManager::init(const boost::property_tree::ptree::value_type &component)
{
    AppSettings &params = AppSettings::getInstance();
    params.configure(component.second);

    m_controllerPtr = (params.enableController) ? 
        std::shared_ptr<CubeControllerItf>(new CubeController(params.pins)) :
        std::shared_ptr<CubeControllerItf>(new StubCubeController(params.pins));

    subscribe(
        "updateCube",
        std::bind(&CubeManager::updateCube, this, std::placeholders::_1));
    subscribe(
        "updateLed",
        std::bind(&CubeManager::updateLed, this, std::placeholders::_1));
}

void CubeManager::start()
{
    AppSettings &params = AppSettings::getInstance();
    uint32_t cyclePeriodUs = (params.cyclePeriodMs * 1000) / m_numLayers;

    LOG_INFO(DOMAIN, "Starting timer for updating layers with period: [%d us]", cyclePeriodUs);

    m_timerCycleId = setPeriodicTimer(
        std::bind(&CubeManager::periodicUpdate, this, std::placeholders::_1),
        std::chrono::microseconds(cyclePeriodUs));

    if (params.statePublishPeriodMs != 0)
    {
        LOG_INFO(DOMAIN, "Starting timer for publishing state: [%d us]", params.statePublishPeriodMs);

        m_timerStateId = setPeriodicTimer(
            std::bind(&CubeManager::publishState, this, std::placeholders::_1),
            std::chrono::microseconds(cyclePeriodUs));
    }
}

void CubeManager::stop()
{
    cancelTimer(m_timerCycleId);
    cancelTimer(m_timerStateId);
    m_controllerPtr.reset();
}

void CubeManager::periodicUpdate(const boost::system::error_code &e)
{
    ++m_activeLayer;
    if (m_activeLayer >= m_numLayers)
    {
        m_activeDigit = m_activeDigit << 1;
        m_activeDigit = (m_activeDigit == 0) ? 1 : m_activeDigit;
        m_activeLayer = 0;
    }

    const uint8_t* data = m_cubeData.getLayerData(m_activeLayer);
    bool layerState[192];
    bool *layerData = &layerState[0];

    for (uint8_t i = 0; i < 96; i++)
    {
        *(layerData++) = (data[i] & m_activeDigit) != 0;
        *(layerData++) = (data[i+96] & m_activeDigit) != 0;
    }

    m_controllerPtr->setLayerOutput(m_activeLayer, layerState);
}

void CubeManager::publishState(const boost::system::error_code &e)
{
    core::MessageData attrs;
    attrs.set("data", m_cubeData.getContent().toString());
    post("cubeState", attrs);
}

void CubeManager::updateCube(const core::MessageData &attrs)
{
    std::string data;
    attrs.get("data", data);

    util::math::Cube<util::graphics::Color> cubeData(1);
    cubeData.fromString(data);

    m_cubeData.setContent(cubeData);
}

void CubeManager::updateLed(const core::MessageData &attrs)
{
    std::string data;
    attrs.get("data", data);

    common::message::type::LedState ledState;
    ledState.fromString(data);

    m_cubeData.setState(ledState.x, ledState.y, ledState.z, ledState.color);
}