/**
 * @file ErrorLogger.hpp
 *
 * @brief Logger that outputs data to the console.
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#ifndef _CORE_LOGGER_IMPL_ERROR_LOGGER_HPP_
#define _CORE_LOGGER_IMPL_ERROR_LOGGER_HPP_

#include <cstdint>
#include <string>

#include <boost/circular_buffer.hpp>
#include <boost/property_tree/ptree.hpp>

#include <core/logger/Logger.hpp>

namespace util
{
    namespace logger
	{

        /** Error logger class */
        class ErrorLogger: public Logger
        {
            // Construction
            public:
                ErrorLogger(boost::property_tree::ptree::value_type &params);
                virtual ~ErrorLogger();

            // Methods
            public:
                void log_event(const LogEntryCPtr& entry);

            private:
                /** Flush the current buffer to a file
                 * @param[in] entry which has caused the flush
                 * */
                void flush_buffer(const LogEntryCPtr& entry);

                /**
                 *  @return the biggest id found in file
                 */
                uint32_t get_next_id(const LogEntryCPtr& entry);

            // Encapsulated data
            protected:
                /** Prefix to be added to the file name */
                std::string m_file_prefix;

                /** Suffix to be added to the file name */
                std::string m_file_suffix;

                /** The path to the directory where the log messages are stored */
                std::string m_file_path;

                /** Events */
                boost::circular_buffer<LogEntryCPtr> m_entries;

        };
    }
}

#endif /* _CORE_LOGGER_IMPL_ERROR_LOGGER_HPP_ */
