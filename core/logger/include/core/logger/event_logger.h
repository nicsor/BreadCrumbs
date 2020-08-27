/**
 * @file event_logger.h
 *
 * @brief C Interface to logging methods.
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#ifndef _CORE_UTIL_EVENT_LOGGER_H_
#define _CORE_UTIL_EVENT_LOGGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdint.h>

/** The supported log levels */
enum LogLevel {
	LOG_LEVEL_PANIC,   ///< Log level panic
	LOG_LEVEL_ERROR,   ///< Log level error
	LOG_LEVEL_WARNING, ///< Log level warning
	LOG_LEVEL_INFO,    ///< Log level info
	LOG_LEVEL_DEBUG    ///< Log level debug
};

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOG_PANIC(...)   log_event(__FILENAME__, __LINE__, LOG_LEVEL_PANIC,   __VA_ARGS__)
#define LOG_ERROR(...)   log_event(__FILENAME__, __LINE__, LOG_LEVEL_ERROR,   __VA_ARGS__)
#define LOG_WARNING(...) log_event(__FILENAME__, __LINE__, LOG_LEVEL_WARNING, __VA_ARGS__)
#define LOG_INFO(...)    log_event(__FILENAME__, __LINE__, LOG_LEVEL_INFO,    __VA_ARGS__)
#define LOG_DEBUG(...)   log_event(__FILENAME__, __LINE__, LOG_LEVEL_DEBUG,   __VA_ARGS__)

/**
 * Initialization method for the global logger.
 * This method should be called before attempting to log an event.
 * The method shall be called only once.
 *
 *  @param configFile Path to the configuration file
 */
void logger_init(const char* configFile);

/**
 * Deinitialization method for the global logger.
 */
void logger_deinit();

/**
 * Generic method for logging an event.
 *
 *  @param fileName Name of the file from which the log function was called
 *  @param lineNumber Line number from which the log function was called
 *  @param logLevel Log level
 *  @param logDomain Log domain (used for filtering)
 *  @param logMessage Log message format
 *  @param ... Log message parameters
 */
void log_event(
        const char* fileName,
        const uint32_t lineNumber,
        const LogLevel logLevel,
        const char* logDomain,
        const char* logMessage,
        ...);

#ifdef __cplusplus
}
#endif

#endif /* _CORE_UTIL_EVENT_LOGGER_H_ */
