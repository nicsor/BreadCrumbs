/**
 * @file Point3d.hpp
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#ifndef _UTIL_GRAPHICS_POINT3D_H_
#define _UTIL_GRAPHICS_POINT3D_H_

#include <cstdint>


namespace util
{
	namespace graphics
	{
		template<class T>
        struct Point3D
        {
            T x;
            T y;
            T z;
            Point3D(T x, T y, T z) :
                x(x),
                y(y),
                z(z)
            {}
        };
    }
}

#endif /* _UTIL_GRAPHICS_POINT3D_H_ */
