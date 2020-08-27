/**
 * @file ConsoleLogger.hpp
 *
 * @brief Logger that outputs data to the console.
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#ifndef _CORE_LOGGER_IMPL_CONSOLE_LOGGER_HPP_
#define _CORE_LOGGER_IMPL_CONSOLE_LOGGER_HPP_

#include <core/logger/Logger.hpp>

namespace util
{
    namespace logger
	{

        /** Console Logger class */
        class ConsoleLogger: public Logger {
            public :
            void log_event(const util::logger::LogEntryCPtr& param);
            ~ConsoleLogger();
        };
    }
}

#endif /* _CORE_LOGGER_IMPL_CONSOLE_LOGGER_HPP_ */
