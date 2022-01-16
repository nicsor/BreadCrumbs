/**
 * @file CubeData.cpp
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#include "CubeData.hpp"

#include <core/logger/event_logger.h>

using namespace apps::cubeManager;

namespace
{
    const uint32_t DIMENSION = 8;
    const uint32_t NBR_OF_CHANNELS = 3;
    const uint32_t ROW_SIZE = DIMENSION * NBR_OF_CHANNELS;
    const uint32_t LAYER_SIZE = DIMENSION * ROW_SIZE;
    const char * DOMAIN = "CubeData";
    const uint8_t remapping[192] =
    {
        191,190,189,188,187,186,185,184,183,182,181,180,179,178,177,176,167,166,165,164,163,162,161,160,
        159,158,157,156,155,154,153,152,151,150,149,148,147,146,145,144,168,169,170,171,172,173,174,175,
        143,142,141,140,139,138,137,136,135,134,133,132,131,130,129,128,119,118,117,116,115,114,113,112,
        111,110,109,108,107,106,105,104,103,102,101,100, 99, 98, 97, 96,120,121,122,123,124,125,126,127,
         95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80, 71, 70, 69, 68, 67, 66, 65, 64,
         63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 72, 73, 74, 75, 76, 77, 78, 79,
         47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 23, 22, 21, 20, 19, 18, 17, 16,
         15, 14, 13, 12, 11, 10,  9,  8,  7,  5,  4,  6,  3,  2,  1,  0, 24, 25, 26, 27, 28, 29, 30, 31,
    };
}

static inline uint32_t getDataOffset(uint8_t x, uint8_t y, uint8_t z, uint8_t channel)
{
    return LAYER_SIZE * y + remapping[ROW_SIZE * z + x * NBR_OF_CHANNELS + channel];
}

CubeData::CubeData()
{
    m_data.resize((uint32_t)DIMENSION * DIMENSION * DIMENSION * NBR_OF_CHANNELS);
}

const uint8_t* CubeData::getLayerData(uint8_t layer)
{
    uint32_t offset = LAYER_SIZE * layer;
    return reinterpret_cast<uint8_t*>(&m_data[offset]);
}

util::graphics::Color CubeData::getState(uint32_t x, uint32_t y, uint32_t z)
{
    util::graphics::Color color;
    color.rgb.G = m_data[getDataOffset(x, y, z, 0)];
    color.rgb.R = m_data[getDataOffset(x, y, z, 1)];
    color.rgb.B = m_data[getDataOffset(x, y, z, 2)];

    return color;
}

void CubeData::setState(uint32_t x, uint32_t y, uint32_t z, const util::graphics::Color &color)
{
    m_data[getDataOffset(x, y, z, 0)] = color.rgb.G;
    m_data[getDataOffset(x, y, z, 1)] = color.rgb.R;
    m_data[getDataOffset(x, y, z, 2)] = color.rgb.B;
}

util::math::Cube<util::graphics::Color> CubeData::getContent()
{
    util::math::Cube<util::graphics::Color> cube(DIMENSION);

    // do it without the loop
    for (uint32_t i = 0; i < DIMENSION; ++i)
    {
        for(uint32_t j = 0; j < DIMENSION; ++j)
        {
            for(uint32_t l = 0; l < DIMENSION; ++l)
            {
                cube.set(i, j, l, getState(i, j, l));
            }
        }
    }

    return cube;
}

void CubeData::setContent(util::math::Cube<util::graphics::Color> &data)
{
    if (data.size() != DIMENSION)
    {
        LOG_ERROR(DOMAIN, "Invalid cube size[%d], expecting[%d]", data.size(), DIMENSION);
        return;
    }

    // Todo: redo without offset calculation
    for (uint32_t i = 0; i < DIMENSION; ++i)
    {
        for(uint32_t j = 0; j < DIMENSION; ++j)
        {
            for(uint32_t l = 0; l < DIMENSION; ++l)
            {
                const auto &color = data(i, j, l);
                setState(i, j, l, color);
            }
        }
    }
}