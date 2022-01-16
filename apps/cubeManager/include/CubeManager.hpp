/**
 * @file CubeManager.hpp
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#ifndef _APPS_CUBEMANAGER_CUBEMANAGER_
#define _APPS_CUBEMANAGER_CUBEMANAGER_

#include <gpiod.h>
#include <memory>

#include <core/Component.hpp>

#include "CubeController.hpp"
#include "CubeData.hpp"


class CubeManager : public core::Component
{
private:
    CubeManager();

public:
    CubeManager *clone() const override;
    void init(const boost::property_tree::ptree::value_type &config) override;

    void start() override;
    void stop() override;

protected:
    void periodicUpdate(const boost::system::error_code &e);
    void publishState(const boost::system::error_code &e);
    void updateCube(const core::MessageData &attrs);
    void updateLed(const core::MessageData &attrs);

private:
    static CubeManager _prototype;

    std::shared_ptr<CubeControllerItf> m_controllerPtr;
    core::TimerId m_timerCycleId;
    core::TimerId m_timerStateId;

    CubeData m_cubeData;
    uint8_t m_numLayers;

    uint8_t m_activeLayer;
    uint8_t m_activeDigit;
};
#endif /* _APPS_CUBEMANAGER_CUBEMANAGER_ */