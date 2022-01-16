/**
 * @file AppSettings.cpp
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#include <core/logger/event_logger.h>

#include "AppSettings.hpp"

using namespace apps::cubeManager;

namespace
{
    const char * DOMAIN = "AppSettings";
}

AppSettings AppSettings::m_instance;

void AppSettings::configure(const boost::property_tree::ptree& configuration)
{
    enableController = configuration.get<bool>("enable-gpios");
    LOG_INFO(DOMAIN, "Gpios enabled: [%s]", enableController ? "yes" : "no");

    cyclePeriodMs = configuration.get<uint32_t>("cycle-period-ms");
    LOG_INFO(DOMAIN, "Cycle period: [%d ms]", cyclePeriodMs);

    statePublishPeriodMs = configuration.get<uint32_t>("state-publish-period-ms", statePublishPeriodMs);
    LOG_INFO(DOMAIN, "State publish period: [%d ms]", statePublishPeriodMs);

    if (enableController)
    {
        pins.A0   = configuration.get<uint8_t>("pins.A0"  );
        pins.A1   = configuration.get<uint8_t>("pins.A1"  );
        pins.A2   = configuration.get<uint8_t>("pins.A2"  );
        pins.ST   = configuration.get<uint8_t>("pins.ST"  );
        pins.OE   = configuration.get<uint8_t>("pins.OE"  );
        pins.CLK  = configuration.get<uint8_t>("pins.CLK" );
        pins.DatA = configuration.get<uint8_t>("pins.DatA");
        pins.DatB = configuration.get<uint8_t>("pins.DatB");

        LOG_INFO(DOMAIN, "Pin configuration: A0[%d] A2[%d] DatA[%d] DatB[%d] CLK[%d] ST[%d] OE[%d]",
            pins.A0, pins.A1, pins.A2, pins.DatA, pins.DatB, pins.CLK, pins.ST, pins.OE);
    }
}