// Header file
#include "./glinit.hpp"

// STD
#include <iostream>

// OpenGL
#include <glad/glad.h>

// Initializes Qt Gui application
QGuiApplication glengine::initQGuiApplication(int argc, char *argv[]) {
  // Initializing Qt Gui application
  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  return QGuiApplication{argc, argv};
}

// Terminates Qt Gui application
void glengine::terminateQGuiApplication(QGuiApplication &app) {
  // Closing Qt Gui application
  app.quit();
}

// Initializes GLFW
int glengine::initGLFW() {
  if (glfwInit() == GLFW_FALSE) {
    std::cout << "error: failed to initialize GLFW" << std::endl;

    return 1;
  }

  return 0;
}

// Terminates GLFW
void glengine::terminateGLFW() { glfwTerminate(); }

// Initializes configured window with OpenGL context
GLFWwindow *glengine::createWindow(int width, int height, const std::string &title,
                                   int openGLVersionMajor, int openGLVersionMinor) {
  // Setting OpenGL version and profile
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, openGLVersionMajor);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, openGLVersionMinor);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);

  // Creating GLFW window
  GLFWwindow *window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "error: failed to create GLFW window" << std::endl;

    glfwTerminate();
    return nullptr;
  }

  // Capturing OpenGL context
  glfwMakeContextCurrent(window);

  // Loading OpenGL functions with GLAD
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cout << "error: failed to load OpenGL functions using GLAD" << std::endl;

    glfwMakeContextCurrent(nullptr);
    glfwTerminate();
    return nullptr;
  }

  // Releasing OpenGL context
  glfwMakeContextCurrent(nullptr);

  // Returning window
  return window;
}

// Terminates window with OpenGL context
void glengine::terminateWindow(GLFWwindow *window) { glfwSetWindowShouldClose(window, true); }

// Enables fullscreen mode for given window
void glengine::enableFullscreenMode(GLFWwindow *window) {
  GLFWmonitor       *monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode *mode    = glfwGetVideoMode(monitor);
  glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
}

// Enables fullscreen mode for given window
void glengine::enableFullscreenMode(GLFWwindow *window, int &posX, int &posY, int &width,
                                    int &height) {
  glfwGetWindowPos(window, &posX, &posY);
  glfwGetWindowSize(window, &width, &height);
  enableFullscreenMode(window);
}

// Disables fullscreen mode for given window
void glengine::disableFullscreenMode(GLFWwindow *window, int posX, int posY, int width,
                                     int height) {
  GLFWmonitor       *monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode *mode    = glfwGetVideoMode(monitor);
  if (width == 0 || height == 0) {
    width  = mode->width;
    height = mode->height;
  }
  glfwSetWindowMonitor(window, nullptr, posX, posY, width, height, mode->refreshRate);
}
