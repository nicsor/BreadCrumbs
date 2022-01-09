/**
 * @file ConsoleLogger.cpp
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#include <iostream>

#include <boost/format.hpp>

#include <core/util/Time.hpp>
#include <core/logger/impl/ConsoleLogger.hpp>

namespace util
{
    namespace logger
    {
        void ConsoleLogger::log_event(const util::logger::LogEntryCPtr& param)
        {
            // Formatted output
            boost::format logMsg("[%s][%s] %s: %s (%s:%u)\n");
            logMsg % core::util::time::to_string(param->get_timestamp())
                   % logger_level_map(param->get_level())
                   % param->get_domain()
                   % param->get_message()
                   % param->get_filename()
                   % param->get_line();

            std::cout << param->get_message() << " " << param->get_filename() << ":" << param->get_line()<< std::endl;
        }

        ConsoleLogger::~ConsoleLogger()
        {
        }

    }
}
