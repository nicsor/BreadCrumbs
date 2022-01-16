#ifndef _APPS_CUBE3D_APPSETTINGS_
#define _APPS_CUBE3D_APPSETTINGS_

#include <stdint.h>
#include <vector>

#include <GL/glut.h>
#include <GLFW/glfw3.h>

#include <boost/property_tree/ptree.hpp>

#include <util/graphics/Color.hpp>
#include <util/graphics/Point.hpp>
#include <util/graphics/Point3d.hpp>

class AppSettings
{
public:
    static AppSettings& getInstance()
    {
        return m_instance;
    }

public:

    struct 
    {
        bool mousePressed = false;
        bool moveDirection = true;
        util::graphics::Color selectedColor = 0;
        uint32_t blinkCounter = 0;
        
        struct
        {
            util::graphics::Point3D<uint32_t> selectedLed{0,0,0};
            util::graphics::Point3D<double> rotation{0,0,0};
            util::graphics::Point<double> mouseDownPosition{0,0};
        } previous;
        
        struct
        {
            util::graphics::Point3D<uint32_t> selectedLed{0,0,0};
            util::graphics::Point3D<double> rotation{0,0,0};
        } current;

        util::graphics::Point3D<double> translation{0,0,0};
        util::graphics::Point3D<double> angle{0,0,0};
        double scalingFactor = 1;

        GLFWwindow *window;
    } state;

    struct 
    {
        struct {
            uint32_t unitMs = 100;
            uint32_t on = 9;
            uint32_t off = 1;
        } blinkPeriod;

        uint32_t renderIntervalMs = 30;

        struct {
            bool fullscreen = true;
            uint32_t width = 1280;
            uint32_t height = 960;
        } screen;

        struct {
            uint32_t diameter = 8;
            uint32_t slices = 25;
            float spaceing = 80;
        } sphere;

        double scalingFactor = 0.02;
        double mouseRotationRange = 50;
        double rotationFactor = 1;
        double translationFactor = 1;        
    } config;

    void configure(const boost::property_tree::ptree& config);

private:
    AppSettings()
    {}

private:
    static AppSettings m_instance;
};

#endif /* _APPS_CUBE3D_APPSETTINGS_ */
