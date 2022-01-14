/**
 * @file AppSettings.hpp
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#ifndef _APPS_CUBEMANAGER_APPSETTINGS_
#define _APPS_CUBEMANAGER_APPSETTINGS_

#include <cstdint>

#include <boost/property_tree/ptree.hpp>

#include "CubeController.hpp"

class AppSettings
{
public:
    static AppSettings& getInstance()
    {
        return m_instance;
    }

private:
    AppSettings() :
        pins{0},
        cyclePeriodMs(5),
        enableController(true)
    {        
    }

public:
    void configure(const boost::property_tree::ptree& config);

public:
    CubePinout pins;
    uint32_t cyclePeriodMs;
    bool enableController;

private:
    static AppSettings m_instance;
};

#endif /* _APPS_CUBEMANAGER_APPSETTINGS_ */
