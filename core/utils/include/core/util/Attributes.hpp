/**
 * @file Attributes.hpp
 *
 * @brief Generic container class for storing various types.
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#ifndef _CORE_UTIL_ATTRIBUTES_H_
#define _CORE_UTIL_ATTRIBUTES_H_

#include <any>
#include <map>
#include <optional>
#include <stdexcept>
#include <string>

namespace core
{   
    namespace util
    {
        /** Generic container class. */
        class Attributes
        {
        private:
            std::map<const std::string, std::any> m_attributes; ///< Attributes

            // Construction
        public:
            Attributes()
            {
            }

            template<typename T>
            Attributes(const std::string &key, const T &value)
            {
                set(key, value);
            }

            // Methods
        public:

			/**
			 *  @param[in] key
			 *  @param[in] value
			 */
            template<typename T>
            Attributes &operator()(const std::string &key, T &value)
            {
                set(key,value);
                return *this;
            }

            /**
             * @param[in] key to retrieve
             * @return value associated with the key, if it exists
             * 
             * @throws out_of_range | bad_any_cast exception if the key does not
             *            exist or is of a different type
             */
            template<typename T>
            T get(const std::string &key) const
            {
                return std::any_cast<T>(m_attributes.at(key));
            }

            /**
             * @param[in] key to retrieve
             * @param[out] value associated with the key, if it exists
             * 
             * @throws out_of_range | bad_any_cast exception if the key does not
             *            exist or is of a different type
             */
            template<typename T>
            void get(const std::string &key, T &value) const
            {
                value = get<T>(key);
            }

            /**
             * @param[in] key to retrieve
             * @param[out] value associated with the key
             * @return true if a key of the specified type exists, false otherwise
             */
            template<typename T>
            bool get_optional(const std::string &key, std::optional<T> &value) const
            {
                try
                {
                    value = std::optional<T>(std::any_cast<T>(m_attributes.at(key)));
                }
                catch (const std::out_of_range &e)
                {
                    value = std::nullopt;
                    return false;
                }
                catch (const std::bad_any_cast &e) {
                    value = std::nullopt;
                    return false;
                }
 
                return true;
            }
            
            /**
             * @param[in] key to store
             * @param[out] value to associate with the given key
             * @return refrence to the updated object
             */
            template<typename T>
            Attributes set(const std::string &key, const T &value)
            {
                m_attributes[key] = std::any(value);
                return *this;
            }

            /** @return iterator pointing to the first element in the sequence */
            std::map<const std::string, std::any>::iterator begin() noexcept;

            /** @return iterator pointing to the past-the-end element in the sequence */
            std::map<const std::string, std::any>::iterator end() noexcept;

            /** @return number of attributes */
            size_t count();
        };
    }
}

#endif /* _CORE_UTIL_ATTRIBUTES_H_ */