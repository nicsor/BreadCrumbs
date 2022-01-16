/**
 * @file Color.hpp
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#ifndef _UTIL_GRAPHICS_COLOR_H_
#define _UTIL_GRAPHICS_COLOR_H_

#include <cstdint>

#include <boost/serialization/serialization.hpp>

namespace util
{
	namespace graphics
	{
        union Color
        {
            struct
            {
                uint8_t alpha;
                uint8_t R;
                uint8_t G;
                uint8_t B;
            } rgb;
            uint32_t color;
            Color() { color = 0; }
            Color(uint32_t val) { color = val; }

    		friend class boost::serialization::access;
			template <class Archive>
			void serialize(Archive &ar, const unsigned int version) {
                ar &color; 
            }
        };
	}
}

BOOST_CLASS_IMPLEMENTATION(util::graphics::Color, boost::serialization::object_serializable)

#endif /* _UTIL_GRAPHICS_COLOR_H_ */
