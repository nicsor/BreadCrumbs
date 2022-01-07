/**
 * @file Component.hpp
 *
 * @brief This is the base class for a component.
 *        It implements the mechanism to keep track of all
 *        components that have been linked inside a build.
 *        It offers a message dispatcher through which
 *        components may interact
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#ifndef _CORE_COMPONENT_COMPONENT_H_
#define _CORE_COMPONENT_COMPONENT_H_

#include <any>
#include <functional>
#include <cstdint>
#include <string>
#include <map>

#include <boost/property_tree/ptree.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/signals2.hpp>

#include <core/util/Attributes.hpp>

namespace core
{
	// Poorly defined types ... TO FIX
	typedef std::string MessageId;
	typedef core::util::Attributes MessageData;
	typedef boost::signals2::signal<void (MessageData)> Signal;
	typedef boost::ptr_map<std::string, Signal>  SignalMap;
	typedef std::function<void (const MessageData&)> MessageHandler;

	namespace
	{
		const uint32_t MAX_NBR_OF_COMPONENT = 64;
		const uint32_t MAX_COMPONENT_NAME_LEN = 32;
	}

	/** Component class */
	class Component {

		public:
			// Construction
			Component();
			virtual ~Component();

			// Methods

			/**
			 * Adds an entry in the map
			 *
			 *  @param[in] type name of prototype
			 *  @param[in] p an instance of the class
			 */
			static Component* addPrototype(const char* type, Component* p);

			/**
			 *  Creates a clone of the selected component
			 *
			 *   @param[in] component parameters of the object
			 */
			static Component* makeComponent(const boost::property_tree::ptree::value_type& component);

			/**
			 * Method to implement specific steps for cloning a component
			 *
			 *  @param[in] component parameters of the object
			 */
			virtual Component* clone(const boost::property_tree::ptree::value_type& component) = 0;

			/** Method to signal that all components have been initialized */
			virtual void start();

			/** Method to signal the stop of a component */
			virtual void stop();

			/** Method to permit a component to subscribe to a message-id */
			void subscribe(const MessageId& messageName, MessageHandler handler);

			/** Method to publish a message-id */
			void post(const MessageId& messageName, const MessageData& message);

		protected:

			/** Map of all registered signals */
			static SignalMap m_signal_map;

			/** Array of available component names */
			static char m_components_name[MAX_NBR_OF_COMPONENT][MAX_COMPONENT_NAME_LEN];

			/** Array of base instances of each available component */
			static Component* m_components_base[MAX_NBR_OF_COMPONENT];

			/** Available components */
			static uint32_t m_components_count;
	};
}

#endif /* _CORE_COMPONENT_COMPONENT_H_ */
