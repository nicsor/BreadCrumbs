/**
 * @file EnumCast.hpp
 *
 * @brief Helper class to permit the mapping of enums to strings.
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#ifndef _CORE_UTIL_ENUM_CAST_H_
#define _CORE_UTIL_ENUM_CAST_H_

#include <core/logger/event_logger.h>
#include <map>

namespace core
{
	namespace util
	{
		/** Associative class containing two members: a value and its corresponding string */
		template<typename T>
		class EnumCast {
			std::map<T, std::string> enum_map;
			std::map<std::string, T> string_map;

		public:
			EnumCast()
			{
			}

			/**
			 *  @param[in] a the value
			 *  @param[in] s the corresponding string
			 */
			EnumCast(T a, const std::string &s)
			{
				add_pair(a, s);
			}

			/** Pairs the value a and the string s */
			void add_pair(T a, const std::string &s)
			{
				enum_map.insert(std::make_pair(a, s));
				string_map.insert(std::make_pair(s, a));
			}

			/** Overload the function operator to pair the value a with the string s */
			EnumCast &operator()(T a, const std::string &s)
			{
				add_pair(a, s);
				return *this;
			}

			/** @return the value corresponding to the string s */
			T operator()(const std::string &s) const
					{
				auto temp = string_map.find(s);
				return temp->second;
			}

			/** @return the string corresponding to the value a */
			std::string operator()(T a) const
					{
				auto temp = enum_map.find(a);
				return temp->second;
			}

		};
	}
}

#endif /* _CORE_UTIL_ENUM_CAST_H_ */
