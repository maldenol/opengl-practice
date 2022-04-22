// Header file
#include "./glinit.hpp"

// STD
#include <iostream>

// OpenGL
#include <glad/glad.h>

// Initializes Qt Gui application
QGuiApplication glservice::initQGuiApplication(int argc, char *argv[]) {
  // Initializing Qt Gui application
  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  return QGuiApplication{argc, argv};
}

// Terminates Qt Gui application
void glservice::terminateQGuiApplication(QGuiApplication &app) {
  // Closing Qt Gui application
  app.quit();
}

// Initializes GLFW
int glservice::initGLFW() {
  // Initializing GLFW
  if (glfwInit() == GLFW_FALSE) {
    std::cout << "error: failed to initialize GLFW" << std::endl;

    return -1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  return 0;
}

// Terminates GLFW
void glservice::terminateGLFW() {
  // Terminating GLFW
  glfwTerminate();
}

// Creates GLFW window and loads OpenGL functions with GLAD
GLFWwindow *glservice::initWindow(int width, int height, const char *title) {
  // Creating a window
  GLFWwindow *window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "error: failed to create GLFW window" << std::endl;
    return nullptr;
  }

  // Capturing OpenGL context
  glfwMakeContextCurrent(window);

  // Loading OpenGL functions using GLAD
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cout << "error: failed to load OpenGL functions using GLAD" << std::endl;

    // Releasing OpenGL context
    glfwMakeContextCurrent(nullptr);

    return nullptr;
  }

  // Releasing OpenGL context
  glfwMakeContextCurrent(nullptr);

  return window;
}

// Terminates GLFW window
void glservice::terminateWindow(GLFWwindow *window) {
  // Making window close
  glfwSetWindowShouldClose(window, true);
}

// Enables fullscreen mode for given window
void glservice::enableFullscreenMode(GLFWwindow *window) {
  GLFWmonitor       *monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode *mode    = glfwGetVideoMode(monitor);
  glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
}

// Enables fullscreen mode for given window
void glservice::enableFullscreenMode(GLFWwindow *window, int &posX, int &posY, int &width,
                                     int &height) {
  glfwGetWindowPos(window, &posX, &posY);
  glfwGetWindowSize(window, &width, &height);
  enableFullscreenMode(window);
}

// Disables fullscreen mode for given window
void glservice::disableFullscreenMode(GLFWwindow *window, int posX, int posY, int width,
                                      int height) {
  GLFWmonitor       *monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode *mode    = glfwGetVideoMode(monitor);
  if (width == 0 || height == 0) {
    width  = mode->width;
    height = mode->height;
  }
  glfwSetWindowMonitor(window, nullptr, posX, posY, width, height, mode->refreshRate);
}
