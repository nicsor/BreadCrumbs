/**
 * @file FileLogger.cpp
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#include <algorithm>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <regex>
#include <string>

#include <boost/format.hpp>

#include <core/util/Time.hpp>

#include <core/logger/impl/FileLogger.hpp>

namespace util
{
    namespace logger
	{
        FileLogger::FileLogger(const boost::property_tree::ptree::value_type &config)
        {
            m_file_prefix      = config.second.get<std::string>("file.prefix", "");
            m_file_suffix      = config.second.get<std::string> ("file.suffix", "");
            m_file_path        = config.second.get<std::string>("file.path", "");
            m_max_nbr_of_files = config.second.get<uint32_t>("max.files", 0);
            m_max_file_size_b  = config.second.get<uint32_t>("max.file_size", 0);
            m_current_file     = get_file_path(0);
            m_current_index    = 0;

            initialize_fs();
        }

        std::string FileLogger::get_file_path(uint32_t index) const
        {
            boost::format filename("%s%d%s");
            filename % this->m_file_prefix
                     % index
                     % this->m_file_suffix;

            std::filesystem::path path(this->m_file_path);
            path /= filename.str();

            return path.string();
        }

        void FileLogger::log_event(const util::logger::LogEntryCPtr& param)
        {
            // Prepare the log message
            std::string message;
            {
                boost::format logMsg("[%s][%s] %s: %s (%s:%u)\n");
                logMsg  % core::util::time::to_string(param->get_timestamp())
                        % logger_level_map(param->get_level())
                        % param->get_domain()
                        % param->get_message()
                        % param->get_filename()
                        % param->get_line();

                message = logMsg.str();
            }

            // Trim the message if it exceeds the maximum log file size
            if ((message.length() > (this->m_max_file_size_b)) and (this->m_max_file_size_b > 0))
            {
                message = message.substr(0, this->m_max_file_size_b);
            }

            // If the file exists and is full
            if (std::filesystem::exists(m_current_file) and
                (not ((std::filesystem::file_size(m_current_file) + message.length())
                            <= (m_max_file_size_b)) or (m_max_file_size_b == 0)))
            {
                // Ensure that the directory exists. Maybe the user removed it.
                std::filesystem::create_directories(this->m_file_path);

                this->m_current_index++;

                if (this->m_max_nbr_of_files > 0)
                {
                   uint32_t oldestIndex = this->m_current_index - this->m_max_nbr_of_files;

                   // Remove the oldest file
                   std::filesystem::remove(get_file_path(oldestIndex));
                }

                // Make sure the current file is removed so that
                // the next message is written in an empty file
                m_current_file = get_file_path(this->m_current_index);
                std::filesystem::remove(m_current_file);
            }

            // Commit the changes to the disk
            std::ofstream file;
            file.open(m_current_file, std::ios_base::app);
            file << message;
            file.close();
        }

        void FileLogger::initialize_fs()
        {
            // Create the log path if it does not exist
            std::filesystem::create_directories(m_file_path);

            std::vector<uint32_t> indexes;

            // Get list of available log indexes
            {
                std::filesystem::path path(this->m_file_path);

                // Check all the existing files in the current path
                if (std::filesystem::exists(path))
                {
                    std::regex pattern(
                        "^" + this->m_file_prefix + "([0-9]*)" + this->m_file_suffix + "$");

                    for (const auto &entry : std::filesystem::directory_iterator(path))
                    {
                        if (std::filesystem::is_regular_file(entry))
                        {
                            std::string fileName = entry.path().filename().string();
                            std::smatch match;

                            if (std::regex_search(fileName, match, pattern))
                            {
                                uint32_t index = std::stoul(match[1]);
                                indexes.push_back(index);
                            }
                        }
                    }
                }

                std::sort(indexes.begin(), indexes.end());
            }

            if (not indexes.empty())
            {
                // If there is a gap larger than m_max_nr_of_files,
                // we consider that we have found the latest file
                uint32_t i;
                for (i = 0; i < indexes.size() - 1; i++)
                {
                    if ((indexes.at(i + 1) - indexes.at(i)) >= (this->m_max_nbr_of_files))
                    {
                        break;
                    }
                }

                m_current_index = i;
                m_current_file = get_file_path(this->m_current_index);

                // Remove old logs
                {
                    uint32_t maxIndex = m_current_index;
                    uint32_t minIndex = m_current_index - m_max_nbr_of_files + 1;

                    // We consider that there was an overflow
                    if (minIndex > maxIndex)
                    {
                        for (uint32_t index : indexes)
                        {
                            if ((index < minIndex) and (index > maxIndex))
                            {
                                std::filesystem::remove(get_file_path(index));
                            }
                        }
                    }
                    else
                    {
                        for (uint32_t index : indexes)
                        {
                            if ((index < minIndex) or (index > maxIndex))
                            {
                                std::filesystem::remove(get_file_path(index));
                            }
                        }
                    }
                }
            }
        }

        FileLogger::~FileLogger()
        {
        }
    }
}
