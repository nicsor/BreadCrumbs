/**
 * @file DetachedLogger.cpp
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#include <iostream>

#include <boost/format.hpp>

#include <core/logger/event_logger.h>
#include <core/util/Time.hpp>

#include <core/logger/impl/DetachedLogger.hpp>

namespace util
{
    namespace logger
    {
        void DetachedLogger::log_event(const LogEntryCPtr& entry)
        {
            if (not m_stop)
            {
                std::unique_lock lock(m_consumer_mutex);
                m_msg_queue.push(entry);
                m_cond_variable.notify_one();
            }
        }

        void DetachedLogger::handler()
        {
            while(not m_stop)
            {
                std::unique_lock lock(m_consumer_mutex);

                while(m_msg_queue.empty())
                {
                    m_cond_variable.wait(lock);
                    if (m_msg_queue.empty() and m_stop)
                    {
                        std::cout << "handler exited\n";
                        return;
                    }
                }

                const LogEntryCPtr entry = m_msg_queue.front();

                // Formatted output
                boost::format logMsg("Detached: [%s][%s] %s: %s (%s:%u)\n");
                logMsg % core::util::time::to_string(entry->get_timestamp())
                       % logger_level_map(entry->get_level())
                       % entry->get_domain()
                       % entry->get_message()
                       % entry->get_filename()
                       % entry->get_line();

                std::cout << logMsg.str();
          
                m_msg_queue.pop();
            }
        }

        DetachedLogger::DetachedLogger()
        {
            std::scoped_lock lock(this->m_consumer_mutex);

            m_stop = false;
            m_thread = std::thread(&DetachedLogger::handler, this);
        }

        DetachedLogger::~DetachedLogger()
        {
            if (not m_stop) {
                // Set stop flag
                m_stop = true;

                // Wake up the thread
                m_cond_variable.notify_one();

                // Join the thread
                if (this->m_thread.joinable()) {
                    this->m_thread.join();
                }
            }
        }
    }
}
