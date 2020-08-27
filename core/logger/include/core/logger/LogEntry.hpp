/**
 * @file Logger.hpp
 *
 * @brief Basic structure for a logger entry
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#ifndef _CORE_LOGGER_LOG_ENTRY_H_
#define _CORE_LOGGER_LOG_ENTRY_H_

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>

#include <core/logger/event_logger.h>
#include <core/util/EnumCast.hpp>

namespace util
{
    namespace logger
    {
		static const core::util::EnumCast<LogLevel> logger_level_map = core::util::EnumCast<LogLevel>
				(LogLevel::LOG_LEVEL_PANIC,   "panic")
				(LogLevel::LOG_LEVEL_ERROR,   "error")
				(LogLevel::LOG_LEVEL_WARNING, "warning")
				(LogLevel::LOG_LEVEL_INFO,    "info")
				(LogLevel::LOG_LEVEL_DEBUG,   "debug");

        /**
         * Structure used to pass parameters to a logger
         */
        struct LogEntry {
        private:
            const LogLevel m_level;          ///< Log level
            const std::string m_message_str; ///< Log message
            const std::string m_domain;      ///< Domain
            const uint32_t m_line;           ///< Line
            const std::string m_filename;    ///< Filename
            /* Log timestamp, using UTC time */
            const std::chrono::system_clock::time_point m_timepoint;

        public:
            /**
             *  @param[in] logLevel  Specifies the level at which the log will be registered
             *  @param[in] message   The message that will be logged
             *  @param[in] domain    The name of the domain under which the message was logged
             *  @param[in] timestamp The time at which the event has occurred
             *  @param[in] line      The line at which the event was registered
             *  @param[in] filename  The from which the event was raised
             */
            LogEntry(
                    const LogLevel logLevel,
                    const std::string &message,
                    const std::chrono::system_clock::time_point &timepoint,
                    const std::string &domain,
                    const uint32_t line,
                    const std::string &filename) :
                            m_level(logLevel),
                            m_message_str(message),
                            m_domain(domain),
                            m_line(line),
                            m_filename(filename),
                            m_timepoint(timepoint)
            {
            }

            /** Get log level */
            LogLevel get_level() const
            {
                return m_level;
            }

            /** Get message */
            const std::string& get_message() const
            {
                return m_message_str;
            }

            /** Get timestamp */
            const std::chrono::system_clock::time_point& get_timestamp() const
            {
                return m_timepoint;
            }

            /** Get domain */
            const std::string& get_domain() const
            {
                return m_domain;
            }

            /** Get line */
            uint32_t get_line() const
            {
                return m_line;
            }

            /** Get file name */
            const std::string& get_filename() const
            {
                return m_filename;
            }
        };

        typedef std::shared_ptr<const LogEntry> LogEntryCPtr;
    }
}
#endif /* _CORE_LOGGER_LOG_ENTRY_H_ */
