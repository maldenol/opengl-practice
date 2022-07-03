#ifndef GLENGINE_GLINIT_HPP
#define GLENGINE_GLINIT_HPP

// Qt5
#include <QGuiApplication>

// OpenGL
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace glengine {

// Initializes Qt Gui application
QGuiApplication initQGuiApplication(int argc, char *argv[]);

// Terminates Qt Gui application
void terminateQGuiApplication(QGuiApplication &app);

// Initializes GLFW
int initGLFW();
// Terminates GLFW
void terminateGLFW();

// Initializes configured window with OpenGL context
GLFWwindow *createWindow(int width, int height, const std::string &title, int openGLVersionMajor,
                         int openGLVersionMinor);
// Terminates window with OpenGL context
void terminateWindow(GLFWwindow *window);

// Enables fullscreen mode for given window
void enableFullscreenMode(GLFWwindow *window);
// Enables fullscreen mode for given window
void enableFullscreenMode(GLFWwindow *window, int &posX, int &posY, int &width, int &height);
// Disables fullscreen mode for given window
void disableFullscreenMode(GLFWwindow *window, int posX = 0, int posY = 0, int width = 0,
                           int height = 0);

}  // namespace glengine

#endif
