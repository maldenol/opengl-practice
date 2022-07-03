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

// GLM
#include <glm/gtx/euler_angles.hpp>

// "glservice" internal library
#include <glservice.hpp>

using namespace glservice;

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
float                gCurrTime{};
float                gDeltaTime{};
PerspectiveCamera    gCamera{};
Camera6DoFController gCameraController{&gCamera};
int                  gPolygonMode{};

// GLFW callbacks
void framebufferSizeCallback(GLFWwindow *window, int width, int height);

// Functions for controls processing
void cursorPosCallback(GLFWwindow *window, double posX, double posY);
void scrollCallback(GLFWwindow *window, double offsetX, double offsetY);
void processUserInput(GLFWwindow *window);

// Main function
int main(int argc, char *argv[]) {
  // Initializing Qt Gui application
  QGuiApplication app = initQGuiApplication(argc, argv);

  // Initializing GLFW and getting configured window with OpenGL context
  initGLFW();
  GLFWwindow *window =
      createWindow(kWidth, kHeight, "triangle", kOpenGLVersionMajor, kOpenGLVersionMinor);

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
  std::vector<GLuint> lightShaderTypes{
      GL_VERTEX_SHADER,
      GL_FRAGMENT_SHADER,
  };
  // Creating arrays of filenames of shaders
  std::vector<QString> objectShaderFilenames{
      getAbsolutePathRelativeToExecutable("objectVS.glsl"),
      getAbsolutePathRelativeToExecutable("objectFS.glsl"),
  };
  std::vector<QString> lightShaderFilenames{
      getAbsolutePathRelativeToExecutable("lightVS.glsl"),
      getAbsolutePathRelativeToExecutable("lightFS.glsl"),
  };
  // Creating shader programs
  GLuint objectSP = glCreateProgram();
  GLuint lightSP  = glCreateProgram();
  // Running shaderWatcher threads
  std::mutex        glfwContextMutex{};
  std::atomic<bool> objectShaderWatcherIsRunning = true;
  std::atomic<bool> objectShadersAreRecompiled   = false;
  std::thread       objectShaderWatcherThread{shaderWatcher,
                                        std::cref(objectShaderWatcherIsRunning),
                                        std::ref(objectShadersAreRecompiled),
                                        window,
                                        std::ref(glfwContextMutex),
                                        objectSP,
                                        std::cref(objectShaderTypes),
                                        std::cref(objectShaderFilenames)};
  std::atomic<bool> lightShaderWatcherIsRunning = true;
  std::atomic<bool> lightShadersAreRecompiled   = false;
  std::thread       lightShaderWatcherThread{shaderWatcher,
                                       std::cref(lightShaderWatcherIsRunning),
                                       std::ref(lightShadersAreRecompiled),
                                       window,
                                       std::ref(glfwContextMutex),
                                       lightSP,
                                       std::cref(lightShaderTypes),
                                       std::cref(lightShaderFilenames)};

  // Loading textures
  std::vector<std::vector<Texture>> textures{
      std::vector<Texture>{                                          },
      std::vector<Texture>{
                           Texture{0, loadTexture("albedoMap.png")}, Texture{1, loadTexture("normalMap.png")},
                           Texture{2, loadTexture("heightMap.png")},
                           Texture{3, loadTexture("ambientOcclusionMap.png")},
                           Texture{4, loadTexture("roughnessMap.png")},
                           //Texture{5, loadTexture("emissionMap.png")},
      },
  };

  // Creating and configuring scene objects
  std::vector<SceneObject> sceneObjects{};
  sceneObjects.push_back(SceneObject{
      glm::vec3{   0.0f, -1.0f, 0.0f},
      glm::vec3{  90.0f,  0.0f, 0.0f},
      glm::vec3{  20.0f, 10.0f, 30.0f},
      std::shared_ptr<BaseLight>{nullptr      },
      std::make_shared<Mesh>(generatePlane(1.0f, 1, objectSP, textures[1]))
  });
  sceneObjects.push_back(SceneObject{
      glm::vec3{   0.0f,  2.0f, 0.0f},
      glm::vec3{  90.0f,  0.0f, 0.0f},
      glm::vec3{  20.0f, 10.0f, 30.0f},
      std::shared_ptr<BaseLight>{nullptr      },
      std::make_shared<Mesh>(generatePlane(1.0f, 1, objectSP, textures[1]))
  });
  sceneObjects[sceneObjects.size() - 1].meshPtr->material.glossiness = 5.0f;
  sceneObjects.push_back(SceneObject{
      glm::vec3{   0.1f,   0.1f, 0.1f},
      glm::vec3{ 180.0f, 180.0f, 180.0f},
      glm::vec3{   2.0f,   2.0f, 2.0f},
      std::shared_ptr<BaseLight>{nullptr       },
      std::make_shared<Mesh>(generateCube(0.5f, 10, false, objectSP, textures[1]))
  });
  sceneObjects[sceneObjects.size() - 1].meshPtr->material.glossiness = 10.0f;
  sceneObjects.push_back(SceneObject{
      glm::vec3{   0.0f, 10.0f, 0.0f},
      glm::vec3{   0.0f,  0.0f, 0.0f},
      glm::vec3{   1.0f,  1.0f, 1.0f},
      std::make_shared<DirectionalLight>(glm::vec3{   0.2f,  0.2f, 0.2f},
      glm::vec3{   0.5f, -1.0f, 0.0f}
      ),
      std::shared_ptr<Mesh>{nullptr      }
  });
  sceneObjects.push_back(SceneObject{
      glm::vec3{0.0f, 0.8f, -1.0f},
      glm::vec3{0.0f, 0.0f,  0.0f},
      glm::vec3{1.0f, 1.0f,  1.0f},
      std::make_shared<PointLight>(glm::vec3{1.0f, 0.0f,  1.0f},
      0.045f, 0.0075),
      std::make_shared<Mesh>(generateQuadSphere(0.1f, 10, true, lightSP, textures[0]))
  });
  sceneObjects.push_back(SceneObject{
      glm::vec3{-0.1f, 0.75f, -0.1f},
      glm::vec3{ 0.0f, 90.0f,  0.0f},
      glm::vec3{ 1.0f,  1.0f,  1.0f},
      std::make_shared<SpotLight>(glm::vec3{ 0.0f,  1.0f,  0.0f},
      glm::vec3{ 0.6f, -1.0f,  0.9f},
      0.045f,
                                  0.0075, 15.0f, 13.0f),
      std::make_shared<Mesh>(generateUVSphere(0.1f, 10, lightSP, textures[0]))
  });
  sceneObjects.push_back(SceneObject{
      glm::vec3{0.1f,  1.0f, 0.1f},
      glm::vec3{0.0f,  0.0f, 0.0f},
      glm::vec3{1.0f,  1.0f, 1.0f},
      std::make_shared<SpotLight>(glm::vec3{1.0f,  1.0f, 0.0f},
      glm::vec3{0.3f, -1.0f, 0.6f},
      0.045f,
                                  0.0075, 30.0f, 25.0f),
      std::make_shared<Mesh>(generateIcoSphere(0.1f, lightSP, textures[0]))
  });

  // Releasing OpenGL context
  glfwMakeContextCurrent(nullptr);

  // Configuring camera and cameraControllers
  gCamera.setPosition(glm::vec3{0.0f, 0.0f, 1.0f});
  gCamera.lookAt(glm::vec3{0.0f, 0.0f, 0.0f});
  // If cameraController is Camera5DoFController
  Camera5DoFController *camera5DoFController =
      dynamic_cast<Camera5DoFController *>(&gCameraController);
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
      glUseProgram(0);

      // Notifying that all routine after object shader recompilation is done
      objectShadersAreRecompiled = false;
    }

    // If light shaders are recompiled
    if (lightShadersAreRecompiled) {
      // Setting uniform values
      glUseProgram(lightSP);
      // make some stuff
      glUseProgram(0);

      // Notifying that all routine after light shader recompilation is done
      lightShadersAreRecompiled = false;
    }

    // Getting light sources
    std::vector<SceneObject> directionalLightSceneObjects{};
    std::vector<SceneObject> pointLightSceneObjects{};
    std::vector<SceneObject> spotLightSceneObjects{};
    for (unsigned int i = 0; i < sceneObjects.size(); ++i) {
      if (sceneObjects[i].lightPtr == nullptr) continue;

      SceneObject &sceneObject = sceneObjects[i];
      BaseLight   *lightPtr    = sceneObject.lightPtr.get();

      // If light is directional
      DirectionalLight *direcionalLight = dynamic_cast<DirectionalLight *>(lightPtr);
      if (direcionalLight != nullptr) {
        directionalLightSceneObjects.push_back(sceneObject);

        continue;
      }

      // If light is point
      PointLight *pointLight = dynamic_cast<PointLight *>(lightPtr);
      if (pointLight != nullptr) {
        pointLightSceneObjects.push_back(sceneObject);

        continue;
      }

      // If light is spot
      SpotLight *spotLight = dynamic_cast<SpotLight *>(lightPtr);
      if (spotLight != nullptr) {
        spotLightSceneObjects.push_back(sceneObject);

        continue;
      }
    }

    // Updating object shader program uniform values
    glUseProgram(objectSP);
    glUniform3fv(glGetUniformLocation(objectSP, "viewPos"), 1,
                 glm::value_ptr(gCamera.getPosition()));
    for (unsigned int i = 0; i < directionalLightSceneObjects.size(); ++i) {
      SceneObject      &sceneObject = directionalLightSceneObjects[i];
      DirectionalLight *direcionalLight =
          dynamic_cast<DirectionalLight *>(sceneObject.lightPtr.get());

      glUniform3fv(glGetUniformLocation(
                       objectSP, ("directionalLights[" + std::to_string(i) + "].color").c_str()),
                   1, glm::value_ptr(direcionalLight->color));
      glm::mat4x4 rotateMatrix{glm::eulerAngleXYZ(glm::radians(sceneObject.rotate.x),
                                                  glm::radians(sceneObject.rotate.y),
                                                  glm::radians(sceneObject.rotate.z))};
      glm::vec3   dir{
          rotateMatrix * glm::vec4{direcionalLight->direction, 0.0f}
      };
      glUniform3fv(glGetUniformLocation(
                       objectSP, ("directionalLights[" + std::to_string(i) + "].dir").c_str()),
                   1, glm::value_ptr(dir));
    }
    for (unsigned int i = 0; i < pointLightSceneObjects.size(); ++i) {
      SceneObject &sceneObject = pointLightSceneObjects[i];
      PointLight  *pointLight  = dynamic_cast<PointLight *>(sceneObject.lightPtr.get());

      glUniform3fv(glGetUniformLocation(
                       objectSP, ("pointLights[" + std::to_string(i) + "].worldPos").c_str()),
                   1, glm::value_ptr(sceneObject.translate));
      glUniform3fv(
          glGetUniformLocation(objectSP, ("pointLights[" + std::to_string(i) + "].color").c_str()),
          1, glm::value_ptr(pointLight->color));
      glUniform1f(glGetUniformLocation(
                      objectSP, ("pointLights[" + std::to_string(i) + "].linAttCoef").c_str()),
                  pointLight->linAttCoef);
      glUniform1f(glGetUniformLocation(
                      objectSP, ("pointLights[" + std::to_string(i) + "].quadAttCoef").c_str()),
                  pointLight->quadAttCoef);
    }
    for (unsigned int i = 0; i < spotLightSceneObjects.size(); ++i) {
      SceneObject &sceneObject = spotLightSceneObjects[i];
      SpotLight   *spotLight   = dynamic_cast<SpotLight *>(sceneObject.lightPtr.get());

      glUniform3fv(glGetUniformLocation(objectSP,
                                        ("spotLights[" + std::to_string(i) + "].worldPos").c_str()),
                   1, glm::value_ptr(sceneObject.translate));
      glUniform3fv(
          glGetUniformLocation(objectSP, ("spotLights[" + std::to_string(i) + "].color").c_str()),
          1, glm::value_ptr(spotLight->color));
      glm::mat4x4 rotateMatrix{glm::eulerAngleXYZ(glm::radians(sceneObject.rotate.x),
                                                  glm::radians(sceneObject.rotate.y),
                                                  glm::radians(sceneObject.rotate.z))};
      glm::vec3   dir{
          rotateMatrix * glm::vec4{spotLight->direction, 0.0f}
      };
      glUniform3fv(
          glGetUniformLocation(objectSP, ("spotLights[" + std::to_string(i) + "].dir").c_str()), 1,
          glm::value_ptr(dir));
      glUniform1f(glGetUniformLocation(
                      objectSP, ("spotLights[" + std::to_string(i) + "].linAttCoef").c_str()),
                  spotLight->linAttCoef);
      glUniform1f(glGetUniformLocation(
                      objectSP, ("spotLights[" + std::to_string(i) + "].quadAttCoef").c_str()),
                  spotLight->quadAttCoef);
      glUniform1f(
          glGetUniformLocation(objectSP, ("spotLights[" + std::to_string(i) + "].angle").c_str()),
          glm::radians(spotLight->angle));
      glUniform1f(glGetUniformLocation(
                      objectSP, ("spotLights[" + std::to_string(i) + "].smoothAngle").c_str()),
                  glm::radians(spotLight->smoothAngle));
    }
    glUseProgram(0);

    // Rendering scene objects
    for (unsigned int i = 0; i < sceneObjects.size(); ++i) {
      if (sceneObjects[i].meshPtr == nullptr) continue;

      SceneObject &sceneObject = sceneObjects[i];
      Mesh        &mesh        = *sceneObject.meshPtr;

      glUseProgram(mesh.shaderProgram);
      glUniform1f(glGetUniformLocation(mesh.shaderProgram, "material.ambCoef"),
                  mesh.material.ambCoef);
      glUniform1f(glGetUniformLocation(mesh.shaderProgram, "material.diffCoef"),
                  mesh.material.diffCoef);
      glUniform1f(glGetUniformLocation(mesh.shaderProgram, "material.specCoef"),
                  mesh.material.specCoef);
      glUniform1f(glGetUniformLocation(mesh.shaderProgram, "material.glossiness"),
                  mesh.material.glossiness);
      glUniform1i(glGetUniformLocation(mesh.shaderProgram, "material.albedoMap"), 0);
      glUniform1i(glGetUniformLocation(mesh.shaderProgram, "material.normalMap"), 1);
      glUniform1i(glGetUniformLocation(mesh.shaderProgram, "material.heightMap"), 2);
      glUniform1i(glGetUniformLocation(mesh.shaderProgram, "material.ambOccMap"), 3);
      glUniform1i(glGetUniformLocation(mesh.shaderProgram, "material.roughMap"), 4);
      glUniform1i(glGetUniformLocation(mesh.shaderProgram, "material.emissMap"), 5);
      glUseProgram(0);

      renderSceneObject(sceneObject, gCamera);
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
  lightShaderWatcherIsRunning = false;
  lightShaderWatcherThread.join();

  // Terminating window with OpenGL context and GLFW
  terminateWindow(window);
  terminateGLFW();

  // Terminating Qt Gui application
  terminateQGuiApplication(app);

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
  Camera5DoFController *camera5DoFController =
      dynamic_cast<Camera5DoFController *>(&gCameraController);
  if (camera5DoFController != nullptr) {
    // Rotating camera
    camera5DoFController->addAngles(glm::radians(offsetX), glm::radians(offsetY));
  }

  // If cameraController is Camera6DoFController
  Camera6DoFController *camera6DoFController =
      dynamic_cast<Camera6DoFController *>(&gCameraController);
  if (camera6DoFController != nullptr) {
    // Rotating camera
    camera6DoFController->rotateRight(glm::radians(offsetY));
    camera6DoFController->rotateUp(glm::radians(offsetX));
  }
}

void scrollCallback(GLFWwindow *window, double offsetX, double offsetY) {
  // Checking if camera is orthographic
  OrthographicCamera *orthoCamera = dynamic_cast<OrthographicCamera *>(&gCamera);
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
  PerspectiveCamera *perspCamera = dynamic_cast<PerspectiveCamera *>(&gCamera);
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
  Camera6DoFController *camera6DoFController =
      dynamic_cast<Camera6DoFController *>(&gCameraController);
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
        enableFullscreenMode(window, sPosX, sPosY, sWidth, sHeight);
      } else {
        disableFullscreenMode(window, sPosX, sPosY, sWidth, sHeight);
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
    terminateWindow(window);
  }

  if (released) {
    sPressed = false;
  }
}
