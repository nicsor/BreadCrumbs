/**
 * @file FileLogger.hpp
 *
 * @brief Logger that outputs data to files, with rotational features.
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#ifndef _CORE_LOGGER_IMPL_FILE_LOGGER_HPP_
#define _CORE_LOGGER_IMPL_FILE_LOGGER_HPP_

#include <boost/property_tree/ptree.hpp>

#include <core/logger/Logger.hpp>

namespace util
{
    namespace logger
	{
        /** File logger class */
        class FileLogger: public Logger
        {
            // Construction
        public:
            /**
             *  @param[in] config Logger configuration details
             */
            FileLogger(const boost::property_tree::ptree::value_type &config);
            virtual ~FileLogger();

            // Methods
        public:
            void log_event(const util::logger::LogEntryCPtr& entry);

            /**
             * @param[in] index The file index
             * @return The path to the file with the specified index.
             */
            std::string get_file_path(uint32_t index) const;

            /**
             * Detect next index and clean old logs.
             */
            void initialize_fs();

            // Encapsulated data
        private:

            /** Prefix to be added to the file name */
            std::string m_file_prefix;

            /** Suffix to be added to the file name */
            std::string m_file_suffix;

            /** The path to the directory where the log messages are stored */
            std::string m_file_path;

            /** Maximum number of files */
            uint32_t m_max_nbr_of_files;

            /** Maximum file size, in bytes */
            uint32_t m_max_file_size_b;

            /** Specifies the current index */
            uint32_t m_current_index;

            /** Specifies the current file */
            std::string m_current_file;
        };
    }
}

#endif /* _CORE_LOGGER_IMPL_FILE_LOGGER_HPP_ */
