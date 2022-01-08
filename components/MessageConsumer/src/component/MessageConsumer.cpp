/**
 * @file MessageConsumer.cpp
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */
#include <functional>
#include <iostream>

#include <boost/format.hpp>

#include <core/logger/event_logger.h>
#include <core/util/Time.hpp>

#include "MessageConsumer.hpp"

namespace component
{
	MessageConsumer MessageConsumer::_prototype;

	MessageConsumer::MessageConsumer()
	{
		Component::addPrototype("MessageConsumer", this);
	}

	void MessageConsumer::handler(const core::MessageData& msg)
	{
		LOG_INFO("dbg",
			"MessageConsumer: received message with value: %u",
			msg.get<uint32_t>("count"));
	}

	MessageConsumer* MessageConsumer::clone() const
	{
		LOG_DEBUG("dbg", "MessageConsumer: cloning consumer");
		return new MessageConsumer(*this);
	}

	void MessageConsumer::init(const boost::property_tree::ptree::value_type& component)
	{
		LOG_DEBUG("dbg", "MessageConsumer: initializing consumer");

		// Clone
		m_published_message = component.second.get<std::string>("msg_id", "");

		if (not m_published_message.empty())
		{
			LOG_DEBUG("dbg", "MessageConsumer: subscribed to message-id: %s", m_published_message.c_str());
			subscribe(m_published_message, std::bind(&MessageConsumer::handler, this, std::placeholders::_1));
		}
	}

	MessageConsumer::~MessageConsumer()
	{
	}

	void MessageConsumer::stop()
	{
		LOG_DEBUG("dbg", "MessageConsumer: stopping consumer");
	}

	void MessageConsumer::start()
	{
		LOG_DEBUG("dbg", "MessageConsumer: starting consumer");
	}
}
