/**
 * @file main.cpp
 *
 * @brief The purpose of this component is to periodically
 *        generate a log and send a message through the system each time.
 *        It is used as demo to highlight how components may be managed
 *        and initialized within an application.
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#include <chrono>
#include <list>
#include <thread>

#include <boost/property_tree/json_parser.hpp>

#include <core/logger/event_logger.h>
#include <core/util/TimeTracker.hpp>
#include <core/Component.hpp>

namespace {
    const char * CONFIG_FILE = "config.json";
}

int main(int argc, char* arcv[])
{
    // List of components
    std::list<core::Component*> componentList;

    // Read config file
    boost::property_tree::ptree root;
    boost::property_tree::read_json(CONFIG_FILE, root);

    // Initialise the logger
    logger_init(CONFIG_FILE);

    // Clone all components
    for (const boost::property_tree::ptree::value_type&
        component : root.get_child("components"))
    {
        componentList.push_back(core::Component::makeComponent(component));
    }

    // Signal that all components have been initialized
    for (core::Component* component : componentList)
    {
        component->start();
    }

    // Sleep for 10 seconds
    std::this_thread::sleep_for(std::chrono::seconds{10});

    // Signal that all components that we are stopping
    for (core::Component* component : componentList)
    {
        component->stop();
        delete component;
    }

    componentList.clear();

    LOG_INFO("dbg", "Timing statistics:\n%s",  core::util::getTimingStatistics().c_str());

    // Uninitialise the logger
    logger_deinit();

    return 0;
}
