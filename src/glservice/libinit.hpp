#ifndef GLSERVICE_LIBINIT_HPP
#define GLSERVICE_LIBINIT_HPP

// Qt5
#include <QGuiApplication>

// OpenGL
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace glservice {

// Initializes Qt Gui application
QGuiApplication initQGuiApplication(int argc, char *argv[]);

// Terminates Qt Gui application
void terminateQGuiApplication(QGuiApplication &app);

// Initializes GLFW
int initGLFW();

// Terminates GLFW
void terminateGLFW();

// Creates GLFW window and loads OpenGL functions with GLAD
GLFWwindow *initWindow(int width, int height, const char *title);

// Terminates GLFW window
void terminateWindow(GLFWwindow *window);

// Enables fullscreen mode for given window
void enableFullscreenMode(GLFWwindow *window);

// Enables fullscreen mode for given window
void enableFullscreenMode(GLFWwindow *window, int &posX, int &posY, int &width, int &height);

// Disables fullscreen mode for given window
void disableFullscreenMode(GLFWwindow *window, int posX = 0, int posY = 0, int width = 0, int height = 0);

}  // namespace glservice

#endif
