/**
 * @file TimeTracker.cpp
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#include <stdint.h>

#include <map>
#include <mutex>

#include <core/util/TimeTracker.hpp>

namespace core {
    namespace util {
		namespace {
			struct TimeInfo {
				uint64_t times_called; ///< Keep track of number of times a particular method was called.
				uint64_t total_time;   ///< Keep track of total execution time for a particular method.
			};

			/** Global map to keep track of method execution */
			std::map<std::string, TimeInfo> tracked_time_info;

			/** Synchronization mutex for updating time */
			std::mutex tracked_time_mutex;
		}

        TimeTracker::TimeTracker(const std::string &id) : m_id(id) {
            m_start = std::chrono::steady_clock::now();
        }

        TimeTracker::~TimeTracker() {
            auto durationUs = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_start).count();

            std::lock_guard<std::mutex> lock(tracked_time_mutex);
            auto item = tracked_time_info.find(m_id);

            if (item == tracked_time_info.end()) {
                tracked_time_info.insert({m_id, {1, (uint64_t)durationUs}});
            } else {
                item->second.total_time += durationUs;
                ++item->second.times_called;
            }
        }

        void clearStatistics() {
            std::lock_guard<std::mutex> lock(tracked_time_mutex);
            tracked_time_info.clear();
        }

        std::string getTimingStatistics() {
             std::string response;
             response.reserve(16 * 1024);

             for (auto &el : tracked_time_info) {
                 auto &stats = el.second;

                 response.append(el.first);
                 response.append(": ");
                 response.append(std::to_string(stats.total_time));
                 response.append("us, ");
                 response.append(std::to_string(stats.times_called));
                 response.append(" times, ");
                 response.append(std::to_string((double) stats.total_time / stats.times_called));
                 response.append("us average\n");
             }

             return response;
        }
    }
}
