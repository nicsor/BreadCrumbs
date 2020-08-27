/**
 * @file DetachedLogger.hpp
 *
 * @brief Logger that queues entries and outputs them to the console from a separate thread.
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#ifndef _CORE_LOGGER_IMPL_DETACHED_LOGGER_HPP_
#define _CORE_LOGGER_IMPL_DETACHED_LOGGER_HPP_

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

#include <core/logger/Logger.hpp>

namespace util
{
    namespace logger
	{

        /** Detached Logger class */
        class DetachedLogger: public Logger {
                // Construction
            public :
                DetachedLogger();
                ~DetachedLogger();
                void log_event(const LogEntryCPtr& param);
            protected:

                /** Function executed in thread. Role of consumer. */
                void handler();

                // Encapsulated data
            private:

                /** Mutex */
                std::mutex m_consumer_mutex;

                /** Thread */
                std::thread m_thread;

                /** Condition variable */
                std::condition_variable m_cond_variable;

                /** Stop flag */
                bool m_stop;

                /** Message queue */
                std::queue<LogEntryCPtr> m_msg_queue;
        };
    }
}

#endif /* _CORE_LOGGER_IMPL_DETACHED_LOGGER_HPP_ */

