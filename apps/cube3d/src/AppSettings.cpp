/**
 * @file AppSettings.cpp
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#include "AppSettings.hpp"

void AppSettings::configure(const boost::property_tree::ptree& configuration)
{
    config.screen.fullscreen = configuration.get<bool>("screen.full", config.screen.fullscreen);
    config.screen.width = configuration.get<uint32_t>("screen.width", config.screen.width);
    config.screen.height = configuration.get<uint32_t>("screen.height", config.screen.height);

    config.sphere.spaceing = configuration.get<uint32_t>("sphere.spacing", config.sphere.spaceing);
    config.sphere.diameter = configuration.get<uint32_t>("sphere.diameter", config.sphere.diameter);
    config.sphere.slices = configuration.get<uint32_t>("sphere.slices", config.sphere.slices);

    config.blinkPeriod.on = configuration.get<uint32_t>("blink-period.on", config.blinkPeriod.on);
    config.blinkPeriod.off = configuration.get<uint32_t>("blink-period.off", config.blinkPeriod.off);
    config.blinkPeriod.unitMs = configuration.get<uint32_t>("blink-period.unit-ms", config.blinkPeriod.unitMs);

    config.translationFactor = configuration.get<double>("translation-factor", config.translationFactor);
    config.rotationFactor = configuration.get<double>("rotation-factor", config.rotationFactor);
    config.scalingFactor = configuration.get<double>("scaling-factor", config.scalingFactor);

    config.renderIntervalMs = configuration.get<int>("renderInterval", config.renderIntervalMs);
    config.mouseRotationRange = configuration.get<uint32_t>("rotation-range", config.mouseRotationRange);
}