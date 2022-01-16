#ifndef _COMPONENTS_COMMON_MESSAGE_TYPE_LEDSTATE_
#define _COMPONENTS_COMMON_MESSAGE_TYPE_LEDSTATE_

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/serialization.hpp>

#include <util/graphics/Color.hpp>

namespace common
{
    namespace message
    {
        namespace type
        {
            struct LedState // TODO: implement serializable
            {
                uint8_t x;
                uint8_t y;
                uint8_t z;

                util::graphics::Color color;

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

                friend class boost::serialization::access;
                template <class Archive>
                void serialize(Archive &ar, const unsigned int version)
                {
                    ar &x;
                    ar &y;
                    ar &z;
                    ar &color;
                }
            };
        }
    }
}

BOOST_CLASS_VERSION(common::message::type::LedState, 1);

#endif /* _COMPONENTS_COMMON_MESSAGE_TYPE_LEDSTATE_ */