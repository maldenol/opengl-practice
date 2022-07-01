// STD
#include <atomic>
#include <chrono>
#include <iostream>
#include <cmath>
#include <mutex>
#include <thread>
#include <vector>

// Qt5
#include <QGuiApplication>
#include <QString>

// OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// "glservice" internal library
#include <glservice.hpp>

// for "ms"
using namespace std::chrono_literals;

// Global constants
static constexpr unsigned int          kWidth               = 800;
static constexpr unsigned int          kHeight              = 600;
static constexpr int                   kOpenGLVersionMajor  = 4;
static constexpr int                   kOpenGLVersionMinor  = 6;
static constexpr std::chrono::duration kRenderCycleInterval = 16ms;
static constexpr float                 kCameraVelocity      = 1.0f;

// Global variables
float                           gCurrTime{};
float                           gDeltaTime{};
glservice::PerspectiveCamera    gCamera{};
glservice::Camera6DoFController gCameraController{&gCamera};
int                             gPolygonMode{};

// GLFW callbacks
void framebufferSizeCallback(GLFWwindow *window, int width, int height);

// Functions for controls processing
void cursorPosCallback(GLFWwindow *window, double posX, double posY);
void scrollCallback(GLFWwindow *window, double offsetX, double offsetY);
void processUserInput(GLFWwindow *window);

// Main function
int main(int argc, char *argv[]) {
  // Initializing Qt Gui application
  QGuiApplication app = glservice::initQGuiApplication(argc, argv);

  // Initializing GLFW and getting configured window with OpenGL context
  GLFWwindow *window = glservice::createWindow(kWidth, kHeight, "triangle", kOpenGLVersionMajor,
                                               kOpenGLVersionMinor);

  // Capturing OpenGL context
  glfwMakeContextCurrent(window);

  // Setting callback functions
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSetCursorPosCallback(window, cursorPosCallback);
  glfwSetScrollCallback(window, scrollCallback);

  // Enabling mouse centering
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Setting OpenGL clear color
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  // Enabling Z-testing
  glEnable(GL_DEPTH_TEST);

  // Creating arrays of types of shaders
  std::vector<GLuint> objectShaderTypes{
      GL_VERTEX_SHADER,
      GL_FRAGMENT_SHADER,
  };
  std::vector<GLuint> sourceShaderTypes{
      GL_VERTEX_SHADER,
      GL_FRAGMENT_SHADER,
  };
  // Creating arrays of filenames of shaders
  std::vector<QString> objectShaderFileNames{
      glservice::getAbsolutePathRelativeToExecutable("objectVS.glsl"),
      glservice::getAbsolutePathRelativeToExecutable("objectFS.glsl"),
  };
  std::vector<QString> sourceShaderFileNames{
      glservice::getAbsolutePathRelativeToExecutable("sourceVS.glsl"),
      glservice::getAbsolutePathRelativeToExecutable("sourceFS.glsl"),
  };
  // Creating shader programs
  GLuint objectSP = glCreateProgram();
  GLuint sourceSP = glCreateProgram();
  // Running shaderWatcher threads
  std::mutex        glfwContextMutex{};
  std::atomic<bool> objectShaderWatcherIsRunning = true;
  std::atomic<bool> objectShadersAreRecompiled   = false;
  std::thread       objectShaderWatcherThread{glservice::shaderWatcher,
                                        std::cref(objectShaderWatcherIsRunning),
                                        std::ref(objectShadersAreRecompiled),
                                        window,
                                        std::ref(glfwContextMutex),
                                        objectSP,
                                        std::cref(objectShaderTypes),
                                        std::cref(objectShaderFileNames)};
  std::atomic<bool> sourceShaderWatcherIsRunning = true;
  std::atomic<bool> sourceShadersAreRecompiled   = false;
  std::thread       sourceShaderWatcherThread{glservice::shaderWatcher,
                                        std::cref(sourceShaderWatcherIsRunning),
                                        std::ref(sourceShadersAreRecompiled),
                                        window,
                                        std::ref(glfwContextMutex),
                                        sourceSP,
                                        std::cref(sourceShaderTypes),
                                        std::cref(sourceShaderFileNames)};

  // Loading textures
  std::vector<std::vector<glservice::Texture>> textures{
      std::vector<glservice::Texture>{                                                                },
      std::vector<glservice::Texture>{
                                      glservice::Texture{0, glservice::loadTexture("albedoMap.png")},glservice::Texture{1, glservice::loadTexture("normalMap.png")},
                                      glservice::Texture{2, glservice::loadTexture("heightMap.png")},
                                      glservice::Texture{3, glservice::loadTexture("ambientOcclusionMap.png")},
                                      glservice::Texture{4, glservice::loadTexture("roughnessMap.png")},
                                      //glservice::Texture{5, glservice::loadTexture("emissionMap.png")},
      },
  };

  // Creating and configuring meshes
  std::vector<glservice::Mesh> meshes{};
  meshes.push_back(glservice::generateCube(1.0f, 10, false, objectSP, textures[1]));
  meshes[meshes.size() - 1].material.glossiness = 5.0f;
  meshes.push_back(glservice::generateCube(1.0f, 10, true, sourceSP, textures[0]));
  meshes[meshes.size() - 1].translate = glm::vec3{3.0f, 2.0f, -2.0f};

  // Releasing OpenGL context
  glfwMakeContextCurrent(nullptr);

  // Configuring camera and cameraControllers
  gCamera.setPosition(glm::vec3{0.0f, 0.0f, 1.0f});
  gCamera.lookAt(glm::vec3{0.0f, 0.0f, 0.0f});
  // If cameraController is Camera5DoFController
  glservice::Camera5DoFController *camera5DoFController =
      dynamic_cast<glservice::Camera5DoFController *>(&gCameraController);
  if (camera5DoFController != nullptr) {
    camera5DoFController->updateLook();
    camera5DoFController->setAngleLimits(0.0f, 0.0f, glm::radians(-85.0f), glm::radians(85.0f));
  }

  // Starting clock
  gCurrTime = static_cast<float>(glfwGetTime());

  // Render cycle
  while (true) {
    // Capturing mutex and OpenGL context
    std::unique_lock glfwContextLock{glfwContextMutex};
    glfwMakeContextCurrent(window);

    // If window should close
    if (glfwWindowShouldClose(window)) break;

    // Processing window events
    glfwPollEvents();

    // Updating clock
    float prevTime = gCurrTime;
    gCurrTime      = static_cast<float>(glfwGetTime());
    gDeltaTime     = gCurrTime - prevTime;

    // Processing user input
    processUserInput(window);

    // Clearing color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // If object shaders are recompiled
    if (objectShadersAreRecompiled) {
      // Setting uniform values
      glUseProgram(objectSP);
      glUniform3f(glGetUniformLocation(objectSP, "ambLightColor"), 1.0f, 1.0f, 1.0f);
      glUniform3f(glGetUniformLocation(objectSP, "light.color"), 1.0f, 1.0f, 1.0f);
      glUseProgram(0);

      // Notifying that all routine after object shader recompilation is done
      objectShadersAreRecompiled = false;
    }

    // If source shaders are recompiled
    if (sourceShadersAreRecompiled) {
      // Setting uniform values
      glUseProgram(sourceSP);
      // make some stuff
      glUseProgram(0);

      // Notifying that all routine after source shader recompilation is done
      sourceShadersAreRecompiled = false;
    }

    // Updating object shader program uniform values
    glUseProgram(objectSP);
    glUniform3fv(glGetUniformLocation(objectSP, "viewPos"), 1,
                 glm::value_ptr(gCamera.getPosition()));
    glUniform3fv(glGetUniformLocation(objectSP, "light.worldPos"), 1,
                 glm::value_ptr(meshes[meshes.size() - 1].translate));
    glUseProgram(0);

    // Rendering meshes
    for (unsigned int i = 0; i < meshes.size(); ++i) {
      glservice::Mesh &mesh = meshes[i];

      glUseProgram(mesh.shaderProgram);
      glUniform1f(glGetUniformLocation(mesh.shaderProgram, "material.ambCoef"),
                  mesh.material.ambCoef);
      glUniform1f(glGetUniformLocation(mesh.shaderProgram, "material.diffCoef"),
                  mesh.material.diffCoef);
      glUniform1f(glGetUniformLocation(mesh.shaderProgram, "material.specCoef"),
                  mesh.material.specCoef);
      glUniform1f(glGetUniformLocation(mesh.shaderProgram, "material.glossiness"),
                  mesh.material.glossiness);
      glUniform1i(glGetUniformLocation(mesh.shaderProgram, "material.texture"), 0);
      glUniform1i(glGetUniformLocation(mesh.shaderProgram, "material.normalMap"), 1);
      glUniform1i(glGetUniformLocation(mesh.shaderProgram, "material.heightMap"), 2);
      glUniform1i(glGetUniformLocation(mesh.shaderProgram, "material.ambOccMap"), 3);
      glUniform1i(glGetUniformLocation(mesh.shaderProgram, "material.roughMap"), 4);
      glUniform1i(glGetUniformLocation(mesh.shaderProgram, "material.emissMap"), 5);
      glUseProgram(0);

      glservice::renderMesh(mesh, gCamera);
    }

    // Swapping front and back buffers
    glfwSwapBuffers(window);

    // Releasing OpenGL context and mutex
    glfwMakeContextCurrent(nullptr);
    glfwContextLock.unlock();

    std::this_thread::sleep_for(kRenderCycleInterval);
  }

  // Waiting for shaderWatchers to stop
  objectShaderWatcherIsRunning = false;
  objectShaderWatcherThread.join();
  sourceShaderWatcherIsRunning = false;
  sourceShaderWatcherThread.join();

  // Terminating window with OpenGL context and GLFW
  glservice::terminateWindow(window);

  // Terminating Qt Gui application
  glservice::terminateQGuiApplication(app);

  return 0;
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  // Setting viewport position and size relative to window
  glViewport(0, 0, width, height);
}

void cursorPosCallback(GLFWwindow *window, double posX, double posY) {
  // Initializaing static variables
  static float sPrevMousePosX{static_cast<float>(posX)};
  static float sPrevMousePosY{static_cast<float>(posY)};

  // Calculating offsets by axies
  float offsetX  = sPrevMousePosX - static_cast<float>(posX);
  float offsetY  = sPrevMousePosY - static_cast<float>(posY);
  sPrevMousePosX = static_cast<float>(posX);
  sPrevMousePosY = static_cast<float>(posY);

  // If cameraController is Camera5DoFController
  glservice::Camera5DoFController *camera5DoFController =
      dynamic_cast<glservice::Camera5DoFController *>(&gCameraController);
  if (camera5DoFController != nullptr) {
    // Rotating camera
    camera5DoFController->addAngles(glm::radians(offsetX), glm::radians(offsetY));
  }

  // If cameraController is Camera6DoFController
  glservice::Camera6DoFController *camera6DoFController =
      dynamic_cast<glservice::Camera6DoFController *>(&gCameraController);
  if (camera6DoFController != nullptr) {
    // Rotating camera
    camera6DoFController->rotateRight(glm::radians(offsetY));
    camera6DoFController->rotateUp(glm::radians(offsetX));
  }
}

void scrollCallback(GLFWwindow *window, double offsetX, double offsetY) {
  // Checking if camera is orthographic
  glservice::OrthographicCamera *orthoCamera =
      dynamic_cast<glservice::OrthographicCamera *>(&gCamera);
  if (orthoCamera != nullptr) {
    // Getting orthographic projection attributes of camera
    float leftBorder{}, rightBorder{}, bottomBorder{}, topBorder{}, nearPlane{}, farPlane{};
    orthoCamera->getProjectionAttributes(leftBorder, rightBorder, bottomBorder, topBorder,
                                         nearPlane, farPlane);

    // Updating border attributes
    leftBorder -= static_cast<float>(glm::radians(offsetY));
    rightBorder += static_cast<float>(glm::radians(offsetY));
    bottomBorder -= static_cast<float>(glm::radians(offsetY));
    topBorder += static_cast<float>(glm::radians(offsetY));

    // Setting orthographic projection attributes of camera
    orthoCamera->setProjectionAttributes(leftBorder, rightBorder, bottomBorder, topBorder,
                                         nearPlane, farPlane);
  }

  // Checking if camera is perspective
  glservice::PerspectiveCamera *perspCamera =
      dynamic_cast<glservice::PerspectiveCamera *>(&gCamera);
  if (perspCamera != nullptr) {
    // Getting perspective projection attributes of camera
    float verticalVOF{}, aspectRatio{}, nearPlane{}, farPlane{};
    perspCamera->getProjectionAttributes(verticalVOF, aspectRatio, nearPlane, farPlane);

    // Updating verticalFOV attribute
    verticalVOF -= static_cast<float>(glm::radians(offsetY));
    if (verticalVOF < glm::radians(1.0f)) {
      verticalVOF = glm::radians(1.0f);
    } else if (verticalVOF > glm::radians(179.0f)) {
      verticalVOF = glm::radians(179.0f);
    }

    // Setting perspective projection attributes of camera
    perspCamera->setProjectionAttributes(verticalVOF, aspectRatio, nearPlane, farPlane);
  }
}

void processUserInput(GLFWwindow *window) {
  static bool sPressed{};
  bool        released{true};

  // Processing movement
  float distance = kCameraVelocity * gDeltaTime;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    gCameraController.moveForward(distance);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    gCameraController.moveForward(-distance);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    gCameraController.moveRight(distance);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    gCameraController.moveRight(-distance);
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    gCameraController.moveUp(distance);
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    gCameraController.moveUp(-distance);
  }

  // If cameraController is Camera6DoFController
  glservice::Camera6DoFController *camera6DoFController =
      dynamic_cast<glservice::Camera6DoFController *>(&gCameraController);
  // Processing movement
  if (camera6DoFController != nullptr) {
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
      camera6DoFController->rotateForward(-distance);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
      camera6DoFController->rotateForward(distance);
    }
  }

  // Toggling fullscreen mode
  if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      static int sPosX{}, sPosY{}, sWidth{}, sHeight{};
      if (glfwGetWindowMonitor(window) == nullptr) {
        glservice::enableFullscreenMode(window, sPosX, sPosY, sWidth, sHeight);
      } else {
        glservice::disableFullscreenMode(window, sPosX, sPosY, sWidth, sHeight);
      }
    }
  }

  // Toggling polygon mode for both sides
  if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      gPolygonMode = (gPolygonMode + 1) % 3;
      switch (gPolygonMode) {
        case 0:
          glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
          glDisable(GL_PROGRAM_POINT_SIZE);
          glPointSize(1);
          break;
        case 1:
          glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
          glDisable(GL_PROGRAM_POINT_SIZE);
          glPointSize(1);
          break;
        case 2:
          glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
          glEnable(GL_PROGRAM_POINT_SIZE);
          glPointSize(10);
          break;
      }
    }
  }

  // Terminating window
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glservice::terminateWindow(window);
    return;
  }

  if (released) {
    sPressed = false;
  }
}
