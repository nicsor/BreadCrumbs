/**
 * @file Component.cpp
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#include <iostream>
#include <thread>

#include <core/logger/event_logger.h>
#include <core/Component.hpp>

namespace core
{
	// Globals
	char Component::m_components_name[MAX_NBR_OF_COMPONENT][MAX_COMPONENT_NAME_LEN];
	Component* Component::m_components_base[MAX_NBR_OF_COMPONENT];
	uint32_t Component::m_components_count = 0;
	SignalMap Component::m_signal_map;

	Component::Component()
	{
	}

	Component::~Component()
	{
		std::vector<TimerId> timerIds;
		std::transform(
			m_timers.begin(),
			m_timers.end(),
			std::back_inserter(timerIds),
			[](auto const &entry) {return entry.first;});

		for (auto& timerId : timerIds)
		{
			cancelTimer(timerId);
		}

		m_timers.clear();
	}

	Component* Component::addPrototype(const char* type, Component* p)
	{
		if (m_components_count < MAX_NBR_OF_COMPONENT)
		{
			// Save the component name and an instance of the prototype
			strncpy(m_components_name[m_components_count], type, MAX_COMPONENT_NAME_LEN);
			m_components_base[m_components_count++] = p;
		}
		else
		{
			// Note: We cannot use the logger at this stage as it requires dynamic memory allocation
			std::cerr << "Too many components configured!";
			exit(-1);
		}

		return p;
	}

	Component* Component::makeComponent(
			const boost::property_tree::ptree::value_type& component)
	{
		std::string type = component.first;

		// Find the component
		Component* temp = nullptr;

		for (int i = 0; i < m_components_count; i++)
		{
			if (strcmp(m_components_name[i], type.c_str()) == 0)
			{
				temp = m_components_base[i];
				break;
			}
		}

		// If the component is not present in the map
		if (temp == nullptr)
		{
			LOG_PANIC("dbg", "Component not found: %s\n", type.c_str());
			for (int i = 0; i < m_components_count; i++)
			{
				LOG_INFO("dbg", "\tAvailable component: %s\n", m_components_name[i]);
			}
			return nullptr;
		}

		return temp->clone(component);

	}

	void Component::start()
	{

	}
	void Component::stop()
	{

	}

	void Component::subscribe(const MessageId& messageName, MessageHandler handler)
	{
		m_signal_map[messageName].connect(handler);
	}

	void Component::post(const MessageId& messageName, const MessageData& message)
	{
		if (m_signal_map.find(messageName) != m_signal_map.end())
		{
			m_signal_map[messageName](message);
		}
	}

	TimerId Component::setOneShotTimer(
		TimeoutHandler handler,
		const std::chrono::steady_clock::duration &duration)
	{
		TimerId timerId = getNextTimerId();
		std::thread thr(
			[this, timerId, handler, duration]
			{
				boost::asio::io_context io;
				m_timers[timerId] = std::make_shared<boost::asio::steady_timer>(io);
				m_timers[timerId]->expires_after(duration);
				m_timers[timerId]->async_wait(handler);

				io.run();
				m_timers.erase(timerId);
			});
		thr.detach();
		return timerId;
	}

	TimerId Component::setPeriodicTimer(
		TimeoutHandler handler,
		const std::chrono::steady_clock::duration &period)
	{
		TimerId timerId = getNextTimerId();
		std::thread thr(
			[this, timerId, handler, period]
			{
				boost::asio::io_context io;
				m_timers[timerId] = std::make_shared<boost::asio::steady_timer>(io);
				m_timers[timerId]->expires_after(period);
				m_timers[timerId]->async_wait(
					boost::bind(
						&Component::handleTimeout,
						this,
						handler,
						timerId,
						period,
						boost::asio::placeholders::error));

				io.run();
			});
		thr.detach();
		return timerId;
	}

	bool Component::cancelTimer(TimerId timerId)
	{
		if (m_timers.count(timerId))
		{
			m_timers[timerId]->cancel();
			m_timers.erase(timerId);

			return true;
		}

		return false;
	}

	TimerId Component::getNextTimerId()
	{
		return ++m_last_timer_id;
	}

	void Component::handleTimeout(
		TimeoutHandler handler,
		const TimerId timerId,
		const std::chrono::steady_clock::duration period,
		const boost::system::error_code &e)
	{
		if (e.value())
		{
			cancelTimer(timerId);
			return;
		}

		handler(e);

		m_timers[timerId]->expires_after(period);
		m_timers[timerId]->async_wait(
			boost::bind(
				&Component::handleTimeout,
				this,
				handler,
				timerId,
				period,
				boost::asio::placeholders::error));
	}
}
