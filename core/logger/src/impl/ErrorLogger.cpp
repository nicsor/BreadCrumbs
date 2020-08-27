/**
 * @file ErrorLogger.cpp
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */


#include <fstream>
#include <filesystem>
#include <regex>

#include <boost/format.hpp>

#include <core/util/Time.hpp>

#include <core/logger/impl/ErrorLogger.hpp>

namespace util
{
    namespace logger
	{

        namespace
        {
            const uint32_t DEFAULT_NBR_OF_ENTRIES = 64;
        }

        ErrorLogger::ErrorLogger(
                boost::property_tree::ptree::value_type &config)
        {
            m_file_path = config.second.get<std::string>("file.path", "");
            m_file_suffix = config.second.get<std::string>("file.suffix", "");
            m_file_prefix = config.second.get<std::string>("file.prefix", "");

            uint32_t maxNbrOfEntries = config.second.get<uint32_t>("max.entries", DEFAULT_NBR_OF_ENTRIES);
            m_entries.set_capacity(maxNbrOfEntries);
        }

        uint32_t ErrorLogger::get_next_id(const LogEntryCPtr& entry)
        {
            uint32_t biggestIdFound = 0;
            std::filesystem::path path(m_file_path);
            path /= entry->get_domain();

            // Check for matching patterns in order to determine the next id
            if (std::filesystem::exists(path))
            {
                const std::regex pattern(
                    m_file_prefix + entry->get_filename() + std::to_string(entry->get_line()) + "_([0-9]*)" + m_file_suffix);

                for (auto fileEntry : std::filesystem::directory_iterator(path))
                {
                    if (std::filesystem::is_regular_file(fileEntry.path()))
                    {
                        std::string filename = fileEntry.path().filename().string();
                        std::cmatch match;

                        if (std::regex_match(filename.c_str(), match, pattern))
                        {
                            uint32_t id = std::stoul(match[1]);

                            if (id > biggestIdFound) {
                                biggestIdFound = id;
                            }
                        }
                    }
                }
            }

            return biggestIdFound + 1;
        }

        void ErrorLogger::flush_buffer(const LogEntryCPtr& entry)
        {
            uint32_t nextIndex = get_next_id(entry);
            std::filesystem::path path(m_file_path);
            path /= entry->get_domain();
            std::filesystem::create_directories(path);

            boost::format fileName("%s%s%u_%u%s");
            fileName  % m_file_prefix
                      % entry->get_filename()
                      % entry->get_line()
                      % nextIndex
                      % m_file_suffix;

            path /= fileName.str();

            std::ofstream file;
            file.open(path, std::ios_base::app);

            for (const auto& logEntry : m_entries)
            {
                // Prepare the log message
                boost::format logMsg("[%s][%s] %s: %s (%s:%u)\n");
                logMsg  % core::util::time::to_string(logEntry->get_timestamp())
                        % logger_level_map(logEntry->get_level())
                        % logEntry->get_domain()
                        % logEntry->get_message()
                        % logEntry->get_filename()
                        % logEntry->get_line();

                file << logMsg.str();
            }

            file.close();
        }

        void ErrorLogger::log_event(const LogEntryCPtr& entry)
        {
            // Push the current entry into our circular queue
            m_entries.push_back(entry);

            if (entry->get_level() <= LogLevel::LOG_LEVEL_ERROR) {
                flush_buffer(entry);
            }
        }

        ErrorLogger::~ErrorLogger()
        {
        }
    }
}
