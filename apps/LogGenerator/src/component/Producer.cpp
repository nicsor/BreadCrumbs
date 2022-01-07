/**
 * @file Producer.cpp
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */
#include <cstdint>

#include <core/logger/event_logger.h>
#include <core/util/TimeTracker.hpp>

#include "Producer.hpp"

namespace component
{
	Producer Producer::_prototype;

	Producer::Producer() :
			m_duration_ms(0), m_stop(true), m_last_published_message_id(0)
	{
		Component::addPrototype("Producer", this);
	}

	Producer::Producer(const Producer &base)
	{
		auto track = core::util::TimeTracker(__PRETTY_FUNCTION__);

		m_duration_ms = base.m_duration_ms;
		m_stop = base.m_stop;
		m_last_published_message_id = base.m_last_published_message_id;
	}

	Producer::~Producer()
	{
		// auto track = core::util::TimeTracker(__PRETTY_FUNCTION__);
		stop();
	}

	Producer* Producer::clone(const boost::property_tree::ptree::value_type& component)
	{
		auto track = core::util::TimeTracker(__PRETTY_FUNCTION__);
		LOG_DEBUG("dbg", "Producer: cloning producer");
		// Local variables
		uint32_t timePeriod = component.second.get<uint32_t>("period", 0);
		std::string timeUnit = component.second.get<std::string>("unit", "milliseconds");
		std::string msgId = component.second.get<std::string>("msg_id", "");

		// Clone
		Producer* clone = new Producer(*this);

		if (timeUnit == "seconds")
		{
			timePeriod *= 1000;
		}
		else
		{
			if (timeUnit != "milliseconds")
			{
				LOG_ERROR(
						"dbg",
						"Invalid time unit configured: [%s] interpreting as ms",
						timeUnit);
			}
		}

		clone->m_duration_ms = timePeriod;
		clone->m_published_message = msgId;

		return clone;
	}

	void Producer::handler(const boost::system::error_code &e)
	{
		if (e)
		{
			return;
		}

		LOG_DEBUG("dbg", "Producer: entered handler");

		auto track = core::util::TimeTracker(__PRETTY_FUNCTION__);
		LOG_ERROR("dbg", "test message ");

		if (not m_published_message.empty())
		{
			core::MessageData attr("count", ++m_last_published_message_id);

			LOG_DEBUG("dbg",
				"Producer: publishing message-id: %s with value: %u",
				m_published_message.c_str(),
				attr.get<uint32_t>("count"));
			post(m_published_message, attr);
		}
	}

	void Producer::stop()
	{
		auto track = core::util::TimeTracker(__PRETTY_FUNCTION__);
		// Scoped lock, used for synchronization
		std::scoped_lock lock(this->m_producer_mutex);
		LOG_DEBUG("dbg", "Producer: stopping producer");

		if (not m_stop)
		{
			// Set stop flag
			m_stop = true;

			// Join the thread
			if (this->m_thread.joinable()) {
				this->m_thread.join();
			}
		}
		LOG_DEBUG("dbg", "Producer: stopped");
	}

	void Producer::start()
	{
		auto track = core::util::TimeTracker(__PRETTY_FUNCTION__);
		std::scoped_lock lock(this->m_producer_mutex);
		LOG_DEBUG("dbg", "Producer: starting producer");

		if (m_stop)
		{
			m_stop = false;

			core::TimeoutHandler handler = std::bind(
					&Producer::handler,
					this,
					std::placeholders::_1);

			setPeriodicTimer(
				handler,
				std::chrono::milliseconds{this->m_duration_ms});
		}
	}
}
