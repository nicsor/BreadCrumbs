/**
 * @file MessageConsumer.hpp
 *
 * @brief The purpose of this component is to intercept a message
 *        published in the system and log it.
 *        It is used as demo to highlight how an external library may be
 *        initialized within an application.
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#ifndef _COMPONENTS_MESSAGE_CONSUMER_H_
#define _COMPONENTS_MESSAGE_CONSUMER_H_

#include <core/Component.hpp>

namespace component
{
    /** Detached Logger class */
    class MessageConsumer: public core::Component {
        private:
            // Construction
            MessageConsumer();
        public :
            ~MessageConsumer();
            // Methods
            MessageConsumer* clone();
            void init(const boost::property_tree::ptree::value_type& component);
            void start();
            void stop();
        protected:

            /** Function executed in thread. Role of consumer. */
            void handler(const core::MessageData& msg);

            // Encapsulated data
        private:
            /** Add the MessageConsumer prototype */
            static MessageConsumer _prototype;

            /** Published message */
            std::string m_published_message;
    };
}

#endif /* _COMPONENTS_MESSAGE_CONSUMER_H_ */

