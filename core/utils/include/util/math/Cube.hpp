/**
 * @file Cube.hpp
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#ifndef _UTIL_MATH_CUBE_H_
#define _UTIL_MATH_CUBE_H_

#include <cstdint>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>

namespace util
{
	namespace math
	{
		template<class T>
		class Cube
		{
		public:
			Cube(int size)
			{
				resize(size);
			}

			T& operator()(uint32_t x, uint32_t y, uint32_t z)
			{
    			uint32_t offset = m_size * m_size * x + m_size * y + z;
    			return m_cubeData.at(offset);
			}

			void resize(int size)
			{
				m_size = size;
				m_cubeData.resize(size*size*size);
			}

			uint32_t size() const
			{
    			return m_size;
			}

			void set(uint32_t x, uint32_t y, uint32_t z, T value)
			{
				uint32_t offset = m_size * m_size * x + m_size * y + z;
				m_cubeData.at(offset) = value;
			}

			std::string toString()
			{
				std::ostringstream oss;
				boost::archive::text_oarchive oa(oss);

				oa << *this;
				return oss.str();
			}

			void fromString(std::string& data)
			{
				std::istringstream iss(data);
				boost::archive::text_iarchive ia(iss);

				ia >> *this;
			}

		private:
			std::vector<T> m_cubeData;
			uint32_t m_size;

    		friend class boost::serialization::access;
			template <class Archive>
			void serialize(Archive &ar, const unsigned int version) {
				ar &m_size; 
				ar &m_cubeData; 
			}
		};
	}
}

#endif /* _UTIL_MATH_CUBE_H_ */