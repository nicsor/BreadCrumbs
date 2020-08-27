/**
 * @file event_logger.cpp
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#include <cstdint>
#include <exception>
#include <mutex>
#include <string>
#include <set>
#include <vector>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <core/util/Time.hpp>

#include "core/logger/impl/ConsoleLogger.hpp"
#include "core/logger/impl/DetachedLogger.hpp"
#include "core/logger/impl/ErrorLogger.hpp"
#include "core/logger/impl/FileLogger.hpp"

#include <core/logger/event_logger.h>

namespace
{
    enum struct LoggerType
    {
        LOGGER_TYPE_DETACHED, ///< Detached console logger
        LOGGER_TYPE_CONSOLE,  ///< Console logger
        LOGGER_TYPE_FILE,     ///< File logger
        LOGGER_TYPE_ERROR     ///< Error logger
    };

    const core::util::EnumCast<LoggerType> logger_type_map =
		core::util::EnumCast<LoggerType>
            (LoggerType::LOGGER_TYPE_FILE,     "file")
            (LoggerType::LOGGER_TYPE_CONSOLE,  "console")
            (LoggerType::LOGGER_TYPE_DETACHED, "detached")
            (LoggerType::LOGGER_TYPE_ERROR,    "error");
    const uint32_t MAX_LOG_MESSAGE_SIZE = 1024;
    bool g_initialized = false;
    LogLevel g_log_level = LogLevel::LOG_LEVEL_ERROR;
    std::vector<util::logger::LoggerPtr> g_loggers;
    std::set<std::string> g_filters;
    std::mutex g_mutex;
}
#include <iostream>
static util::logger::LoggerPtr get_logger(
    boost::property_tree::ptree::value_type &child_tree)
{
    util::logger::LoggerPtr logger;

    LoggerType loggerType = logger_type_map(child_tree.second.get<std::string>("name"));

    switch (loggerType)
    {
        case LoggerType::LOGGER_TYPE_FILE:
            logger = std::make_shared<util::logger::FileLogger>(child_tree);
            break;
        case LoggerType::LOGGER_TYPE_ERROR:
            logger = std::make_shared<util::logger::ErrorLogger>(child_tree);
            break;
        case LoggerType::LOGGER_TYPE_DETACHED:
            std::cout<< "generating instance \n";
            logger = std::make_shared<util::logger::DetachedLogger>();
            std::cout<< "generating instance done \n";
            break;
        case LoggerType::LOGGER_TYPE_CONSOLE:
            logger = std::make_shared<util::logger::ConsoleLogger>();
            break;
        default:
            throw std::invalid_argument("Invalid logger type");
    }

    return logger;
}

void logger_init(const char *configFile)
{
    std::scoped_lock lock(g_mutex);

    if (g_initialized)
    {
        throw std::logic_error("Logger already initialized");
    }

    // Load configuration file
    boost::property_tree::ptree root;
    boost::property_tree::read_json(configFile, root);

    if(root.find("logging") == root.not_found())
    {
        // No logger configured
        return;
    }

    // Get default log level
    g_log_level = util::logger::logger_level_map(
        root.get("logging.level", util::logger::logger_level_map(g_log_level)));

    if(root.find("logging.filters") != root.not_found())
    {
        // Get default filters
        for (boost::property_tree::ptree::value_type &filters : root.get_child("logging.filters"))
        {
            g_filters.insert (filters.second.data());
        }
    }

    // Initialize logging components
    for (boost::property_tree::ptree::value_type &child_tree : root.get_child("logging.consumers"))
    {
        g_loggers.push_back(get_logger(child_tree));
    }

    g_initialized = true;
}

void logger_deinit()
{
    std::scoped_lock lock(g_mutex);

    if (g_initialized) {
        g_log_level = LogLevel::LOG_LEVEL_ERROR;
        g_loggers.clear();
        g_filters.clear();
        g_initialized = false;
    }
}

void log_event(
    const char *fileName,
    const uint32_t lineNumber,
    const LogLevel logLevel,
    const char *logDomain,
    const char *logMessage,
    ...)
{
    static char logMsg[MAX_LOG_MESSAGE_SIZE];

    std::scoped_lock lock (g_mutex);

    // Filter logs
    if ((logLevel > g_log_level) or (g_loggers.size() == 0) or
        ((g_filters.size() > 0) and (g_filters.find(logDomain) == g_filters.end())))
    {
        return;
    }

    // Format the input parameters
    va_list args;
    va_start(args, logMessage);
    vsnprintf(logMsg, MAX_LOG_MESSAGE_SIZE, logMessage, args);
    va_end(args);

    // Prepare the entry to be logged
    util::logger::LogEntryCPtr event =
        std::make_shared<const util::logger::LogEntry>(
            logLevel,
            logMsg,
            core::util::time::get_timestamp(),
            logDomain,
            lineNumber,
            fileName);

    // Publish events to available loggers
    for (const util::logger::LoggerPtr &logger : g_loggers)
    {
        logger->log_event(event);
    }

    if (logLevel == LogLevel::LOG_LEVEL_PANIC)
    {
        // Stop the loggers. Destructor should handle flushing of buffers and clean close.
        g_loggers.clear();

        // Exit the application
        exit(0);
    }
}
