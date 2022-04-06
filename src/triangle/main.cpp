// STD
#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

// Qt5
#include <QGuiApplication>

// Internal library "glengine"
#include <glengine/glengine.hpp>

// for "ms"
using namespace std::chrono_literals;

// Global constants
constexpr unsigned int          kWidth               = 800;
constexpr unsigned int          kHeight              = 600;
constexpr std::chrono::duration kRenderCycleInterval = 16ms;

// Functions for GLFW
void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processUserInput(GLFWwindow *window);

// Main function
int main(int argc, char *argv[]) {
  // Initializing Qt Gui application
  QGuiApplication app = glengine::initQGuiApplication(argc, argv);

  // Initializing GLFW
  glengine::initGLFW();

  // Creating GLFW window and loading OpenGL functions with GLAD
  GLFWwindow *window = glengine::initWindow(kWidth, kHeight, "triangle");

  // Capturing OpenGL context
  glfwMakeContextCurrent(window);

  // Setting callback function on window resize
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  // Creating arrays of shaders, according filenames and types
  std::vector<GLuint>  shaders{0, 0};
  std::vector<QString> shaderFileNames{
      glengine::getAbsolutePathRelativeToExecutable("vertexShader.glsl"),
      glengine::getAbsolutePathRelativeToExecutable("fragmentShader.glsl"),
  };
  std::vector<GLuint> shaderTypes{
      GL_VERTEX_SHADER,
      GL_FRAGMENT_SHADER,
  };
  // Creating and configuring a shader program
  GLuint shaderProgram =
      glengine::initShaderProgram(std::ref(shaders), std::cref(shaderTypes));
  // Running shaderWatcher thread
  std::mutex        glfwContextMutex{};
  std::atomic<bool> shaderWatcherIsRunning = true;
  std::thread       shaderWatcherThread{glengine::shaderWatcher,
                                  std::cref(shaderWatcherIsRunning),
                                  window,
                                  std::ref(glfwContextMutex),
                                  shaderProgram,
                                  std::cref(shaders),
                                  std::cref(shaderFileNames)};

  // Vertices and their indices to make a mesh from triangles
  std::vector<float> vertices{
      -0.5f,  0.5f,  0.0f,
      0.5f,  0.5f,  0.0f,
      -0.25f, -0.5f, 0.0f,
      0.25f, -0.5f, 0.0f,
  };
  std::vector<GLuint> indices{
      0, 1, 2,
      1, 3, 2,
  };
  // Creating and configuring a mesh and getting its VAO
  GLuint vao = glengine::initMesh(std::cref(vertices), std::cref(indices));

  // Releasing OpenGL context
  glfwMakeContextCurrent(nullptr);

  // Render cycle
  while (true) {
    // Capturing mutex and OpenGL context
    std::unique_lock glfwContextLock{glfwContextMutex};
    glfwMakeContextCurrent(window);

    // If window should close
    if (glfwWindowShouldClose(window)) break;

    // Processing window events
    glfwPollEvents();

    // Processing user input
    processUserInput(window);

    // Clearing background
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Drawing mesh
    glengine::drawMesh(vao, vertices.size(), shaderProgram);

    // Swapping front and back buffers
    glfwSwapBuffers(window);

    // Releasing OpenGL context and mutex
    glfwMakeContextCurrent(nullptr);
    glfwContextLock.unlock();

    std::this_thread::sleep_for(kRenderCycleInterval);
  }

  // Waiting for shaderWatcher to stop
  shaderWatcherIsRunning = false;
  shaderWatcherThread.join();

  // Terminating GLFW
  glengine::terminateGLFW();

  // Terminating Qt Gui application
  glengine::terminateQGuiApplication(app);

  return 0;
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  // Setting viewport position and size relative to window
  glViewport(0, 0, width, height);
}

void processUserInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    // Terminating window
    glengine::terminateWindow(window);
    return;
  }

  if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
    // Setting polygon mode for both sides to GL_LINE
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
    // Setting polygon mode for both sides to GL_FILL
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}