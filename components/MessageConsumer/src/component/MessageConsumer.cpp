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
			std::any_cast<uint32_t>(msg));
	}

	MessageConsumer* MessageConsumer::clone(const boost::property_tree::ptree::value_type& component)
	{
		LOG_DEBUG("dbg", "MessageConsumer: cloning consumer");

		// Clone
		MessageConsumer* clone = new MessageConsumer(*this);
		clone->m_published_message = component.second.get<std::string>("msg_id", "");

		if (not m_published_message.empty())
		{
			LOG_DEBUG("dbg", "MessageConsumer: subscribed to message-id: %s", m_published_message.c_str());
			clone->subscribe(m_published_message, std::bind(&MessageConsumer::handler, clone, std::placeholders::_1));
		}

		return clone;
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
