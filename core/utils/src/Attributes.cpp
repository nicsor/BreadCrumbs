
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
        std::map<const std::string, std::any>::iterator Attributes::begin() noexcept
        {
            return m_attributes.begin();
        }

        std::map<const std::string, std::any>::iterator Attributes::end() noexcept
        {
            return m_attributes.end();
        }

        size_t Attributes::count()
        {
            return m_attributes.size();
        }
    }
}