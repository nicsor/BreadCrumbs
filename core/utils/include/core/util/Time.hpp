/**
 * @file Time.hpp
 *
 * @brief Helper class to permit the management of timestamps.
 *        The purpose is to have a common method to get the
 *        current time that could easily be stubbed in tests.
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#include <chrono>
#include <string>

namespace core
{
	namespace util
	{
		namespace time
		{
			std::chrono::system_clock::time_point get_timestamp();
			std::string to_string(const std::chrono::system_clock::time_point& timepoint);
		}
	}
}
