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
#include <condition_variable>
#include <iostream>
#include <filesystem>
#include <execinfo.h>
#include <list>
#include <mutex>
#include <signal.h>
#include <thread>

#include <boost/program_options.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <core/logger/event_logger.h>
#include <core/util/TimeTracker.hpp>
#include <core/Component.hpp>

namespace {
    const char * APP_NAME = "Breadcrumbs";
    const char * CONFIG_FILE = "config.json";

    std::mutex mtx;
    std::condition_variable cv;
    bool shouldExit = false;
}

void handler(int sig) {
    void *array[32];
    size_t size;

    size = backtrace(array, 32);

    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

void closeApp(int sig) {
    shouldExit = true;

    LOG_INFO(APP_NAME, "Recived exit request. Gracefully shutting down ...");

    std::unique_lock<std::mutex> lock(mtx);
    cv.notify_one();
}

int main(int argc, char* arcv[])
{
    signal(SIGSEGV, handler);
    signal(SIGINT, closeApp);

    std::string configFile;
    try
    {
        boost::program_options::options_description options{APP_NAME};
        options.add_options()("help,h", "Help")("file,f", boost::program_options::value<std::string>()->default_value(CONFIG_FILE));

        boost::program_options::variables_map vm;
        boost::program_options::store(boost::program_options::parse_command_line(argc, arcv, options), vm);
        boost::program_options::notify(vm);

        if (vm.count("help"))
        {
            options.print(std::cout);
            return 0;
        }

        configFile = vm["file"].as<std::string>();

        if (not std::filesystem::exists(configFile))
        {
            std::cerr << "Config file[" << configFile << "] does not exist!" << std::endl;
            return -1;
        }
    }
    catch (const boost::program_options::error &e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    // List of components
    std::list<core::Component*> componentList;

    // Read config file
    boost::property_tree::ptree root;
    boost::property_tree::read_json(configFile, root);

    // Initialise the logger
    logger_init(configFile.c_str());

    // Clone all components and initialize them
    for (const boost::property_tree::ptree::value_type&
        component : root.get_child("components"))
    {
        core::Component *comp = core::Component::makeComponent(component);

        componentList.push_back(comp);
        comp->init(component);
    }

    // Signal that all components have been initialized
    for (core::Component* component : componentList)
    {
        component->start();
    }

    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, []{ return shouldExit; });

    // Signal that all components that we are stopping
    for (core::Component* component : componentList)
    {
        component->stop();
        delete component;
    }

    componentList.clear();

    LOG_INFO(APP_NAME, "Timing statistics:\n%s",  core::util::getTimingStatistics().c_str());

    // Uninitialise the logger
    logger_deinit();

    return 0;
}
