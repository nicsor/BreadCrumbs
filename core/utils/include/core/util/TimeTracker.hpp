/**
 * @file Time.hpp
 *
 * @brief Helper class to permit the tracking of execution time.
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#ifndef _CORE_UTIL_TIME_TRACKER_HPP_
#define _CORE_UTIL_TIME_TRACKER_HPP_

#include <chrono>
#include <string>

namespace core {
    namespace util {
    	/** Helper class to keep track of method execution time. */
        class TimeTracker {
            std::string m_id;                              ///< Method id
            std::chrono::steady_clock::time_point m_start; ///< Construction time

            // Construction
         public:
            /**
             * @param[in] methodId String identified for the current entry
             *            ex. __PRETTY_FUNCTION__
             */
            TimeTracker(const std::string &methodId);
            ~TimeTracker();
        };

        /**
         * @return string with statistics summary
         */
        std::string getTimingStatistics();

        /** Clear statistics */
        void clearStatistics();
    }
}

#endif /* _CORE_UTIL_TIME_TRACKER_HPP_ */
