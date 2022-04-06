#ifndef GLENGINE_GLENGINE_HPP
#define GLENGINE_GLENGINE_HPP

// All the headers
#include "filesystem.hpp"
#include "shader.hpp"
#include "mesh.hpp"

// Qt5
#include <QGuiApplication>

// OpenGL
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

// Creates GLFW window and loads OpenGL functions with GLAD
GLFWwindow *initWindow(int width, int height, const char *title);

// Terminates GLFW window
void terminateWindow(GLFWwindow *window);

}  // namespace glengine

#endif