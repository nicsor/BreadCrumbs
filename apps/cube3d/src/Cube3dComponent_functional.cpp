#include "Cube3dComponent.hpp"
#include <core/logger/event_logger.h>

namespace
{
    const char *DOMAIN = "cube3D";
}

AppSettings AppSettings::m_instance;

static void mouseButtonPressCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        AppSettings &instance = AppSettings::getInstance();
        instance.state.mousePressed = (action == GLFW_PRESS);

        if (instance.state.mousePressed)
        {
            glfwGetCursorPos(
                window,
                &instance.state.previous.mouseDownPosition.x,
                &instance.state.previous.mouseDownPosition.y);
        }
        else
        {
            instance.state.previous.rotation = instance.state.current.rotation;
        }
    }
}

static void keyPressCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
        return;
    }

    AppSettings &instance = AppSettings::getInstance();
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch (key)
        {
        case GLFW_KEY_KP_SUBTRACT:
            instance.state.scalingFactor -= instance.config.scalingFactor * instance.state.scalingFactor;
            break;
        case GLFW_KEY_KP_ADD:
            instance.state.scalingFactor += instance.config.scalingFactor * instance.state.scalingFactor;
            break;
        case GLFW_KEY_UP:
            instance.state.current.rotation.y -= instance.config.rotationFactor;
            instance.state.previous.rotation.y = instance.state.current.rotation.y;
            break;
        case GLFW_KEY_DOWN:
            instance.state.current.rotation.y += instance.config.rotationFactor;
            instance.state.previous.rotation.y = instance.state.current.rotation.y;
            break;
        case GLFW_KEY_RIGHT:
            instance.state.current.rotation.x += instance.config.rotationFactor;
            instance.state.previous.rotation.x = instance.state.current.rotation.x;
            break;
        case GLFW_KEY_LEFT:
            instance.state.current.rotation.x -= instance.config.rotationFactor;
            instance.state.previous.rotation.x = instance.state.current.rotation.x;
            break;

        case GLFW_KEY_KP_8: // Translate on y-axis
            instance.state.translation.y += instance.config.translationFactor;
            break;
        case GLFW_KEY_KP_2:
            instance.state.translation.y -= instance.config.translationFactor;
            break;
        case GLFW_KEY_KP_6: // Translate on x-axis
            instance.state.translation.x += instance.config.translationFactor;
            break;
        case GLFW_KEY_KP_4:
            instance.state.translation.x -= instance.config.translationFactor;
            break;
        case GLFW_KEY_KP_7:  // Translate on z-axis
            instance.state.translation.z += instance.config.translationFactor;
            break;
        case GLFW_KEY_KP_9:
            instance.state.translation.z -= instance.config.translationFactor;
            break;

        case GLFW_KEY_Y: // Rotate on y-direction
            instance.state.previous.selectedLed.y += (instance.state.moveDirection == false) ? 1 : -1;
            instance.state.previous.selectedLed.y = instance.state.previous.selectedLed.y & 7;
            break;
        case GLFW_KEY_X: // Rotate on x-direction
            instance.state.previous.selectedLed.x += (instance.state.moveDirection == false) ? 1 : -1;
            instance.state.previous.selectedLed.x = instance.state.previous.selectedLed.x & 7;
            break;
        case GLFW_KEY_Z: // Rotate on z-direction
            instance.state.previous.selectedLed.z += (instance.state.moveDirection == false) ? 1 : -1;
            instance.state.previous.selectedLed.z = instance.state.previous.selectedLed.z & 7;
            break;

        case GLFW_KEY_T:  // Toggle update direction for coordinates/colors
            instance.state.moveDirection = !instance.state.moveDirection;
            break;

        case GLFW_KEY_B: // Update blue channel
        {
            int value = static_cast<uint32_t>(instance.state.selectedColor.rgb.B);
            value += (instance.state.moveDirection) ? instance.config.colorStep : -instance.config.colorStep;
            value = (value <= 0) ? 0 : (value >= 0xFF) ? 0xFF : value;
            instance.state.selectedColor.rgb.B = static_cast<uint8_t>(value);
            break;
        }
        case GLFW_KEY_G: // Update green channel
        {
            int value = static_cast<uint32_t>(instance.state.selectedColor.rgb.G);
            value += (instance.state.moveDirection) ? instance.config.colorStep : -instance.config.colorStep;
            value = (value <= 0) ? 0 : (value >= 0xFF) ? 0xFF : value;
            instance.state.selectedColor.rgb.G = static_cast<uint8_t>(value);
            break;
        }
        case GLFW_KEY_R: // Update red channel
        {
            int value = static_cast<uint32_t>(instance.state.selectedColor.rgb.R);
            value += (instance.state.moveDirection) ? instance.config.colorStep : -instance.config.colorStep;
            value = (value <= 0) ? 0 : (value >= 0xFF) ? 0xFF : value;
            instance.state.selectedColor.rgb.R = static_cast<uint8_t>(value);
            break;
        }

        case GLFW_KEY_0: // Reset color
            instance.state.selectedColor.color=0;
            break;
        }
    }
}

static void manage_mouse_events(GLFWwindow *window)
{
    AppSettings &instance = AppSettings::getInstance();

    if (instance.state.mousePressed)
    {
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);
        instance.state.current.rotation.x = (xPos - instance.state.previous.mouseDownPosition.x) / instance.config.mouseRotationRange + instance.state.previous.rotation.x;
        instance.state.current.rotation.y = (yPos - instance.state.previous.mouseDownPosition.y) / instance.config.mouseRotationRange + instance.state.previous.rotation.y;
    }
}

void Cube3dComponent::printCurrentStatus()
{
    AppSettings &instance = AppSettings::getInstance();
    std::stringstream ss;

    ss << "Selected led: ("
          << instance.state.previous.selectedLed.x << ", "
          << instance.state.previous.selectedLed.y << ", "
          << instance.state.previous.selectedLed.z << ") "
       << "Selected color: (" << std::hex
          << (int)instance.state.selectedColor.rgb.R << ", "
          << (int)instance.state.selectedColor.rgb.G << ", "
          << (int)instance.state.selectedColor.rgb.B << ") "
       << "Direction: " << (instance.state.moveDirection ? "forward" : "backwards");

    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2f(-1, -1);

    for (char c : ss.str())
    {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
    }
}

void Cube3dComponent::drawLine(const GLfloat *point1, const GLfloat *point2, const GLfloat *color)
{
    glLineWidth(0.5);
    glBegin(GL_LINES);
    glColor3fv(color);
    glVertex3fv(point1);
    glVertex3fv(point2);
    glEnd();
}

void Cube3dComponent::drawCube(const AppSettings &instance)
{
    const GLfloat origin[3] = {0, 0, 0};
    const GLfloat offsetX[3] = {instance.config.sphere.spaceing, 0, 0};
    const GLfloat offsetY[3] = {0, instance.config.sphere.spaceing, 0};
    const GLfloat offsetZ[3] = {0, 0, instance.config.sphere.spaceing};
    const GLfloat lineColor[3] = {0.5, 0.5, 0.5};

    for (int x = 0; x < m_cubeData.size(); x++)
    {
        glTranslatef(instance.config.sphere.spaceing, 0, 0);
        glPushMatrix();
        for (int y = 0; y < m_cubeData.size(); y++)
        {
            glTranslatef(0, instance.config.sphere.spaceing, 0);
            glPushMatrix();
            for (int z = 0; z < m_cubeData.size(); z++)
            {
                util::graphics::Color &led = m_cubeData(x, y, z);
                glTranslatef(0, 0, instance.config.sphere.spaceing);

                if (x < m_cubeData.size() - 1) { drawLine(origin, offsetX, lineColor); }
                if (y < m_cubeData.size() - 1) { drawLine(origin, offsetY, lineColor); }
                if (z < m_cubeData.size() - 1) { drawLine(origin, offsetZ, lineColor); }

                glColor3ub(led.rgb.R, led.rgb.G, led.rgb.B);
                glutSolidSphere(instance.config.sphere.diameter, instance.config.sphere.slices, instance.config.sphere.slices);
            }
            glPopMatrix();
        }
        glPopMatrix();
    }
}

bool Cube3dComponent::glInitialize(util::math::Cube<util::graphics::Color> &m_cubeData)
{
    char *myargv[1];
    int myargc = 1;
    myargv[0] = strdup(DOMAIN);
    glutInit(&myargc, myargv);

    if (!glfwInit())
    {
        return false;
    }

    AppSettings &instance = AppSettings::getInstance();
    GLFWmonitor *monitor = nullptr;

    if (instance.config.screen.fullscreen)
    {
        // Get screen resolution
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        instance.config.screen.width = mode->width;
        instance.config.screen.height = mode->height;
    }

    instance.state.window = glfwCreateWindow(
        instance.config.screen.width,
        instance.config.screen.height,
        DOMAIN,
        monitor,
        nullptr);

    if (not instance.state.window)
    {
        glfwTerminate();
        return false;
    }

    // Register callbacks
    glfwSetKeyCallback(instance.state.window, keyPressCallback);
    glfwSetMouseButtonCallback(instance.state.window, mouseButtonPressCallback);

    glfwSetInputMode(instance.state.window, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetInputMode(instance.state.window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

    glfwMakeContextCurrent(instance.state.window);

    glViewport(0.0f, 0.0f, instance.config.screen.width, instance.config.screen.height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, instance.config.screen.width, 0, instance.config.screen.height, 0, 5000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(
        (instance.config.screen.width - instance.config.sphere.spaceing * m_cubeData.size()) / 2,
        (instance.config.screen.height - instance.config.sphere.spaceing * m_cubeData.size()) / 2,
        -2000);

    return true;
}

bool Cube3dComponent::glExecute(util::math::Cube<util::graphics::Color> &m_cubeData)
{
    AppSettings &instance = AppSettings::getInstance();
    bool shouldExit = glfwWindowShouldClose(instance.state.window);

    if (not shouldExit)
    {
        manage_mouse_events(instance.state.window);

        glClear(GL_COLOR_BUFFER_BIT);

        printCurrentStatus();

        glPushMatrix();

        // Update rotation
        glRotatef(instance.state.angle.x, 1, 0, 0);
        glRotatef(instance.state.current.rotation.x, 0, 1, 0);
        glRotatef(instance.state.current.rotation.y, 1, 0, 0);

        // Update scaling
        glScalef(
            instance.state.scalingFactor,
            instance.state.scalingFactor,
            instance.state.scalingFactor);

        // Update translation
        glTranslatef(
            instance.state.translation.x,
            instance.state.translation.y,
            instance.state.translation.z);

        // Draw cube
        drawCube(instance);

        glPopMatrix();

        glfwSwapBuffers(instance.state.window);
        glfwPollEvents();
    }
    else
    {
        glfwTerminate();
    }

    return shouldExit;
}
