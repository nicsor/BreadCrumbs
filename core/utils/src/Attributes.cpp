
/**
 * @file Attributes.cpp
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#include <core/util/Attributes.hpp>

namespace core
{   
    namespace util
    {
        std::map<const std::string, std::any>::const_iterator Attributes::begin() const noexcept
        {
            return m_attributes.begin();
        }

        std::map<const std::string, std::any>::const_iterator Attributes::end() const noexcept
        {
            return m_attributes.end();
        }
        std::map<const std::string, std::any>::const_iterator Attributes::cbegin() const noexcept
        {
            return m_attributes.begin();
        }

        std::map<const std::string, std::any>::const_iterator Attributes::cend() const noexcept
        {
            return m_attributes.end();
        }

        size_t Attributes::count()
        {
            return m_attributes.size();
        }
    }
}