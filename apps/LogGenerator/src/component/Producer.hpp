/**
 * @file Producer.h
 *
 * @brief The purpose of this component is to periodically
 *        generate a log and send a message through the system each time.
 *        It is used as demo to highlight how components may interact.
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#ifndef _LOG_GENERATOR_COMPONENT_PRODUCER_H_
#define _LOG_GENERATOR_COMPONENT_PRODUCER_H_

#include <mutex>
#include <string>
#include <thread>

#include <core/Component.hpp>

namespace component
{
	/** Producer class */
	class Producer: public core::Component
	{
	private:
		// Construction
		Producer();
		Producer(const Producer &base);

	public:

		virtual ~Producer();

		// Methods

		/**
		 * Clone
		 *
		 *   @param[in] component parameters of the object
		 */
		Producer* clone(const boost::property_tree::ptree::value_type& component);

		/** Starts a new thread */
		void start();

		/** Stops the loop */
		void stop();

	protected:

		/** Periodic function */
		void handler(const boost::system::error_code &e);

		// Encapsulated data
	private:

		/** Mutex */
		std::mutex m_producer_mutex;

		/** Thread */
		std::thread m_thread;

		/** Published message */
		std::string m_published_message;

		/** Add the Producer prototype */
		static Producer _prototype;

		/** Duration */
		uint32_t m_duration_ms;

		/** Stop flag */
		bool m_stop;

		/** Last published message id */
		uint32_t m_last_published_message_id;
	};
}

#endif /* _LOG_GENERATOR_COMPONENT_PRODUCER_H_ */
