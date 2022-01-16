/**
 * @file CubeController.hpp
 *
 * @author Nicolae Natea
 * Contact: nicu@natea.ro
 */

#ifndef _APPS_CUBEMANAGER_CUBECONTROLLER_
#define _APPS_CUBEMANAGER_CUBECONTROLLER_

#include <cstdint>
#include <gpiod.h>

namespace apps
{
    namespace cubeManager
    {
        struct CubePinout
        {
            uint8_t A0;
            uint8_t A1;
            uint8_t A2;
            uint8_t DatA;
            uint8_t DatB;
            uint8_t OE;
            uint8_t CLK;
            uint8_t ST;
        };

        class CubeControllerItf
        {
        public:
            virtual ~CubeControllerItf() {}
            virtual void setLayerOutput(uint8_t layer, bool *data) = 0;
        };

        class CubeController : public CubeControllerItf
        {
        public:
            CubeController(const CubePinout &settings);
            virtual ~CubeController();

        public:
            virtual void setLayerOutput(uint8_t layer, bool *data) override;

        protected:
            void activateLayer(uint8_t layer);

        protected:
            struct gpiod_chip *chip;
            struct gpiod_line *lineA0;
            struct gpiod_line *lineA1;
            struct gpiod_line *lineA2;
            struct gpiod_line *lineDatA;
            struct gpiod_line *lineDatB;
            struct gpiod_line *lineOE;
            struct gpiod_line *lineCLK;
            struct gpiod_line *lineST;
        };

        class StubCubeController : public CubeControllerItf
        {
        public:
            StubCubeController(const CubePinout &settings) {}

        public:
            virtual void setLayerOutput(uint8_t layer, bool *data) override {}
        };
    }
}

#endif /* _APPS_CUBEMANAGER_CUBECONTROLLER_ */
