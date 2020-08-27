/**
 * @file Logger.hpp
 *
 * @brief Generic logger interface
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#ifndef _CORE_LOGGER_LOGGER_H_
#define _CORE_LOGGER_LOGGER_H_

#include <memory>
#include <string>

#include "LogEntry.hpp"

namespace util
{
    namespace logger
    {
        /** Common logger interface */
        class Logger {
        public:
            /**
             * Log event
             *
             *  @param[in] entry Event details
             */
            virtual void log_event(const util::logger::LogEntryCPtr& entry) = 0;
            virtual ~Logger()
            {
            }
        };

        typedef std::shared_ptr<Logger> LoggerPtr;
    }
}

#endif /* _CORE_LOGGER_LOGGER_H_ */
