/**
 * @file CubeController.cpp
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#include "CubeController.hpp"

namespace
{
    const char * DOMAIN = "CubeController";
    const bool STUB_FUNCTIONALITY = true;
}

CubeController::CubeController(const CubePinout &pins)
{
    // Open GPIO chip
    chip = gpiod_chip_open_by_name("gpiochip0");

    // Open GPIO lines
    lineA0   = gpiod_chip_get_line(chip, pins.A0);
    lineA1   = gpiod_chip_get_line(chip, pins.A1);
    lineA2   = gpiod_chip_get_line(chip, pins.A2);
    lineDatA = gpiod_chip_get_line(chip, pins.DatA);
    lineDatB = gpiod_chip_get_line(chip, pins.DatB);
    lineOE   = gpiod_chip_get_line(chip, pins.OE);
    lineCLK  = gpiod_chip_get_line(chip, pins.CLK);
    lineST   = gpiod_chip_get_line(chip, pins.ST);

    // Open LED lines for output
    gpiod_line_request_output(lineA0,   DOMAIN, 0);
    gpiod_line_request_output(lineA1,   DOMAIN, 0);
    gpiod_line_request_output(lineA2,   DOMAIN, 0);
    gpiod_line_request_output(lineDatA, DOMAIN, 0);
    gpiod_line_request_output(lineDatB, DOMAIN, 0);
    gpiod_line_request_output(lineOE,   DOMAIN, 0);
    gpiod_line_request_output(lineCLK,  DOMAIN, 0);
    gpiod_line_request_output(lineST,   DOMAIN, 0);
}

CubeController::~CubeController()
{
    gpiod_line_release(lineA0  );
    gpiod_line_release(lineA1  );
    gpiod_line_release(lineA2  );
    gpiod_line_release(lineDatA);
    gpiod_line_release(lineDatB);
    gpiod_line_release(lineOE  );
    gpiod_line_release(lineCLK );
    gpiod_line_release(lineST  );

    gpiod_chip_close(chip);
}

void CubeController::activateLayer(uint8_t layer)
{
    gpiod_line_set_value(lineA0, (layer & 0x1) != 0);
    gpiod_line_set_value(lineA1, (layer & 0x2) != 0);
    gpiod_line_set_value(lineA2, (layer & 0x4) != 0);
}

void CubeController::setLayerOutput(uint8_t layer, bool *data)
{
    // Disable output
    gpiod_line_set_value(lineOE, true);
    activateLayer(layer);
    for (uint8_t i = 0; i < 96; i++)
    {
        // Set state
        gpiod_line_set_value(lineDatB, *(data++));
        gpiod_line_set_value(lineDatA, *(data++));

        // Generate clock transition
        gpiod_line_set_value(lineCLK, false);
        gpiod_line_set_value(lineCLK, true);
    }

    // Set list ST
    gpiod_line_set_value(lineST, false);
    gpiod_line_set_value(lineST, true);

    // Activate output
    gpiod_line_set_value(lineOE, false);
}