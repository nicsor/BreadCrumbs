/**
 * @file Time.cpp
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#include <iomanip>
#include <sstream>

#include <core/util/Time.hpp>

namespace core
{
	namespace util
	{
		namespace time
		{
			std::chrono::system_clock::time_point get_timestamp()
			{
				return std::chrono::system_clock::now();
			}

			std::string to_string(const std::chrono::system_clock::time_point& timepoint)
			{
				std::time_t t = std::chrono::system_clock::to_time_t(timepoint);
				auto gmt_time = gmtime(&t);
				std::stringstream ss;
				ss << std::put_time(gmt_time, "%Y-%m-%d %H:%M:%S");
				return ss.str();
			}
		}
	}
}
