/**
 * @file CubeData.hpp
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#ifndef _APPS_CUBEMANAGER_CUBEDATA_
#define _APPS_CUBEMANAGER_CUBEDATA_

#include <cstdint>
#include <string>

#include <util/math/Cube.hpp>
#include <util/graphics/Color.hpp>

class CubeData
{
public:
    CubeData();
    const uint8_t* getLayerData(uint8_t layer);

    util::graphics::Color getState(uint32_t x, uint32_t y, uint32_t z);
    void setState(uint32_t x, uint32_t y, uint32_t z, const util::graphics::Color &color);

    util::math::Cube<util::graphics::Color> getContent();
    void setContent(util::math::Cube<util::graphics::Color> &data);

private:
    std::string m_data;
};
#endif /* _APPS_CUBEMANAGER_CUBEDATA_ */
