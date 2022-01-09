/**
 * @file Message.hpp
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#ifndef _UTIL_NETWORK_MESSAGE_H_
#define _UTIL_NETWORK_MESSAGE_H_

#include <stdint.h>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>

namespace util
{
    namespace network
    {
        class Message
        {
        public:
            Message(const std::string &id, const std::string &data) : m_sync(0xAA),
                                                                      m_id(id),
                                                                      m_data(data)
            {
                m_checksum = getChecksum();
            }

            Message() : m_sync(0xAA)
            {
                m_checksum = getChecksum();
            }

            std::string getId() const
            {
                return m_id;
            }

            std::string getData() const
            {
                return m_data;
            }

            bool isValid() const
            {
                return (m_sync == 0xAA) and (m_checksum == getChecksum());
            }

            std::string toString()
            {
                std::ostringstream oss;
                boost::archive::text_oarchive oa(oss);

                oa << *this;
                return oss.str();
            }

            void fromString(const std::string &data)
            {
                std::istringstream iss(data);
                boost::archive::text_iarchive ia(iss);

                ia >> *this;
            }

        protected:
            uint8_t getChecksum() const
            {
                uint8_t checksum = 0;
                for (const char &c : m_id)
                {
                    checksum ^= static_cast<uint8_t>(c);
                }

                for (const char &c : m_data)
                {
                    checksum ^= static_cast<uint8_t>(c);
                }

                return checksum;
            }

        private:
            uint8_t m_sync;
            std::string m_id;
            std::string m_data;
            uint8_t m_checksum;

            friend class boost::serialization::access;
            template <class Archive>
            void serialize(Archive &ar, const unsigned int version)
            {
                ar &m_sync;
                ar &m_id;
                ar &m_data;
                ar &m_checksum;
            }
        };
    }
}

#endif /* _UTIL_NETWORK_MESSAGE_H_ */