/**
 * @file Point.hpp
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#ifndef _UTIL_GRAPHICS_POINT_H_
#define _UTIL_GRAPHICS_POINT_H_

#include <stdint.h>


namespace util
{
	namespace graphics
	{
		template<class T>
        struct Point
        {
            T x;
            T y;
            Point(T x, T y) :
                x(x),
                y(y)
            {}
        };
    }
}

#endif /* _UTIL_GRAPHICS_POINT_H_ */