// STD
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <cmath>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

// Qt5
#include <QGuiApplication>

// OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// GLM
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

// "glengine" internal library
#include <glengine.hpp>

using namespace glengine;

// for "ms"
using namespace std::chrono_literals;

// Global constants
static constexpr unsigned int          kWidth               = 800;
static constexpr unsigned int          kHeight              = 600;
static constexpr int                   kOpenGLVersionMajor  = 4;
static constexpr int                   kOpenGLVersionMinor  = 6;
static constexpr std::chrono::duration kRenderCycleInterval = 16ms;
static constexpr float                 kCameraVelocity      = 1.0f;
static constexpr float                 kCameraSprintCoef    = 3.0f;

// Global variables
float             gCurrTime{};
float             gDeltaTime{};
PerspectiveCamera gCamera{};
Controller6DoF    gCameraController{&gCamera};
SceneObject      *gFlashlightSceneObjectPtr{};
int               gPolygonMode{};
bool              gEnableSceneObjectsFloating{true};

// GLFW callbacks
void framebufferSizeCallback(GLFWwindow *window, int width, int height);

// Functions for controls processing
void cursorPosCallback(GLFWwindow *window, double posX, double posY);
void scrollCallback(GLFWwindow *window, double offsetX, double offsetY);
void processUserInput(GLFWwindow *window);

// Makes scene objects float
void floatSceneObjects(std::vector<SceneObject> &sceneObjects, unsigned int startIndex,
                       unsigned int count);

// Main function
int main(int argc, char *argv[]) {
  // Initializing Qt Gui application
  QGuiApplication app = initQGuiApplication(argc, argv);

  // Initializing GLFW and getting configured window with OpenGL context
  initGLFW();
  GLFWwindow *window =
      createWindow(kWidth, kHeight, "3-basic_lighting", kOpenGLVersionMajor, kOpenGLVersionMinor);

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

  // Creating arrays of types of shaders
  std::vector<GLuint> blinnPhongShaderTypes{
      GL_VERTEX_SHADER,
      GL_FRAGMENT_SHADER,
  };
  std::vector<GLuint> lightShaderTypes{
      GL_VERTEX_SHADER,
      GL_FRAGMENT_SHADER,
  };
  // Creating arrays of filenames of shaders
  std::vector<std::string> blinnPhongShaderFilenames{
      getAbsolutePathRelativeToExecutable("blinnPhongVS.glsl"),
      getAbsolutePathRelativeToExecutable("blinnPhongFS.glsl"),
  };
  std::vector<std::string> lightShaderFilenames{
      getAbsolutePathRelativeToExecutable("lightVS.glsl"),
      getAbsolutePathRelativeToExecutable("lightFS.glsl"),
  };
  // Creating shader programs
  GLuint blinnPhongSP = glCreateProgram();
  GLuint lightSP      = glCreateProgram();
  // Running shaderWatcher threads
  std::mutex        glfwContextMutex{};
  std::atomic<bool> blinnPhongShaderWatcherIsRunning = true;
  std::atomic<bool> blinnPhongShadersAreRecompiled   = false;
  std::thread       blinnPhongShaderWatcherThread{shaderWatcher,
                                            std::cref(blinnPhongShaderWatcherIsRunning),
                                            std::ref(blinnPhongShadersAreRecompiled),
                                            window,
                                            std::ref(glfwContextMutex),
                                            blinnPhongSP,
                                            std::cref(blinnPhongShaderTypes),
                                            std::cref(blinnPhongShaderFilenames)};
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
  std::vector<std::vector<std::shared_ptr<Mesh::Material::Texture>>> texturePtrVectors{
      std::vector<std::shared_ptr<Mesh::Material::Texture>>{ },
      std::vector<std::shared_ptr<Mesh::Material::Texture>>{
                                                            std::make_shared<Mesh::Material::Texture>(loadTexture("albedoMap.png", false), 0, false),
                                                            std::make_shared<Mesh::Material::Texture>(loadTexture("normalMap.png", false), 1, false),
                                                            std::make_shared<Mesh::Material::Texture>(loadTexture("depthMap.png", false), 2, false),
                                                            std::make_shared<Mesh::Material::Texture>(loadTexture("ambientOcclusionMap.png", false),
                                                            3, false),
                                                            std::make_shared<Mesh::Material::Texture>(loadTextureHDR("glossinessMap.hdr"), 4, false),
                                                            //std::make_shared<Mesh::Material::Texture>(loadTexture("emissionMap.png", false), 5, false),
                                                            std::make_shared<Mesh::Material::Texture>(loadTexture("black.png", false), 5, false),
                                                            },
  };

  // Creating and configuring scene objects
  // Lower plane
  std::vector<SceneObject> sceneObjects{};
  sceneObjects.push_back(SceneObject{
      glm::vec3{   0.0f,  -1.0f, 0.0f},
      glm::vec3{  90.0f, 180.0f, 0.0f},
      glm::vec3{  20.0f,  10.0f, 30.0f},
      std::shared_ptr<BaseLight>{nullptr       },
      std::make_shared<Mesh>(generatePlane(1.0f, 10, blinnPhongSP, texturePtrVectors[1]))
  });
  sceneObjects[sceneObjects.size() - 1].getMeshPtr()->getMaterialPtr()->setParallaxStrength(0.1f);
  // Upper plane
  sceneObjects.push_back(SceneObject{
      glm::vec3{   0.0f,  2.0f, 0.0f},
      glm::vec3{  90.0f,  0.0f, 0.0f},
      glm::vec3{  20.0f, 10.0f, 30.0f},
      std::shared_ptr<BaseLight>{nullptr      },
      std::make_shared<Mesh>(generatePlane(1.0f, 10, blinnPhongSP, texturePtrVectors[1]))
  });
  sceneObjects[sceneObjects.size() - 1].getMeshPtr()->getMaterialPtr()->setParallaxStrength(0.1f);
  // Central cube
  sceneObjects.push_back(SceneObject{
      glm::vec3{   0.0f,   0.0f, 0.0f},
      glm::vec3{ 180.0f, 180.0f, 180.0f},
      glm::vec3{   2.0f,   2.0f, 2.0f},
      std::shared_ptr<BaseLight>{nullptr       },
      std::make_shared<Mesh>(generateCube(0.5f, 10, false, blinnPhongSP, texturePtrVectors[1]))
  });
  sceneObjects[sceneObjects.size() - 1].getMeshPtr()->getMaterialPtr()->setParallaxStrength(0.1f);
  // Directional light (white)
  sceneObjects.push_back(SceneObject{
      glm::vec3{   0.0f, 10.0f, 0.0f},
      glm::vec3{   0.0f,  0.0f, 0.0f},
      glm::vec3{   1.0f,  1.0f, 1.0f},
      std::make_shared<DirectionalLight>(glm::vec3{   0.2f,  0.2f, 0.2f},
      glm::vec3{   0.5f, -1.0f, 0.0f}
      ),
      std::shared_ptr<Mesh>{nullptr      }
  });
  // Point light (purple)
  sceneObjects.push_back(SceneObject{
      glm::vec3{0.0f, 0.8f, -1.0f},
      glm::vec3{0.0f, 0.0f,  0.0f},
      glm::vec3{1.0f, 1.0f,  1.0f},
      std::make_shared<PointLight>(glm::vec3{0.5f, 0.0f,  0.5f},
      0.45f, 0.075f),
      std::make_shared<Mesh>(generateQuadSphere(0.1f, 10, true, lightSP, texturePtrVectors[0]))
  });
  // Spot light (green)
  sceneObjects.push_back(SceneObject{
      glm::vec3{-0.1f, 0.75f, -0.1f},
      glm::vec3{ 0.0f, 90.0f,  0.0f},
      glm::vec3{ 1.0f,  1.0f,  1.0f},
      std::make_shared<SpotLight>(glm::vec3{ 0.0f,  0.5f,  0.0f},
      glm::vec3{ 0.6f, -1.0f,  0.9f},
      0.45f,
                                  0.075f, glm::radians(15.0f), glm::radians(13.0f)),
      std::make_shared<Mesh>(generateUVSphere(0.1f, 10, lightSP, texturePtrVectors[0]))
  });
  // Spot light (yellow)
  sceneObjects.push_back(SceneObject{
      glm::vec3{0.1f,  1.0f, 0.1f},
      glm::vec3{0.0f,  0.0f, 0.0f},
      glm::vec3{1.0f,  1.0f, 1.0f},
      std::make_shared<SpotLight>(glm::vec3{0.5f,  0.5f, 0.0f},
      glm::vec3{0.3f, -1.0f, 0.6f},
      0.45f,
                                  0.075f, glm::radians(30.0f), glm::radians(25.0f)),
      std::make_shared<Mesh>(generateIcoSphere(0.1f, lightSP, texturePtrVectors[0]))
  });
  // Flashlight
  sceneObjects.push_back(SceneObject{
      glm::vec3{   0.0f, 0.0f, 0.0f},
      glm::vec3{   0.0f, 0.0f, 0.0f},
      glm::vec3{   1.0f, 1.0f, 1.0f},
      std::make_shared<SpotLight>(glm::vec3{   0.7f, 0.7f, 0.7f},
      glm::vec3{   0.0f, 0.0f, 0.0f},
      0.45f,
                                  0.075f, glm::radians(20.0f), glm::radians(18.0f)),
      std::shared_ptr<Mesh>{nullptr     }
  });
  gFlashlightSceneObjectPtr = &sceneObjects[sceneObjects.size() - 1];

  // Releasing OpenGL context
  glfwMakeContextCurrent(nullptr);

  // Configuring camera and cameraControllers
  gCamera.setPosition(glm::vec3{0.0f, 1.0f, 2.0f});
  gCamera.setWorldUp(glm::vec3{0.0f, 1.0f, 0.0f});
  gCamera.lookAt(glm::vec3{0.0f, 0.0f, 0.0f});
  gCamera.setVerticalFOV(glm::radians(60.0f));
  gCamera.setAspectRatio(static_cast<float>(kWidth) / static_cast<float>(kHeight));
  gCamera.setNearPlane(0.01f);
  gCamera.setFarPlane(100.0f);
  // If cameraController is Controller5DoF
  Controller5DoF *camera5DoFControllerPtr = dynamic_cast<Controller5DoF *>(&gCameraController);
  if (camera5DoFControllerPtr != nullptr) {
    camera5DoFControllerPtr->updateLook();
    camera5DoFControllerPtr->setAngleLimits(0.0f, 0.0f, glm::radians(-85.0f), glm::radians(85.0f));
  }

  // Starting clock
  gCurrTime = static_cast<float>(glfwGetTime());

  // Render cycle
  while (true) {
    // Capturing mutex and OpenGL context
    std::unique_lock glfwContextLock{glfwContextMutex};
    glfwMakeContextCurrent(window);

    // If window should close
    if (glfwWindowShouldClose(window)) {
      // Releasing OpenGL context and mutex
      glfwMakeContextCurrent(nullptr);
      glfwContextLock.unlock();

      // Breaking render cycle
      break;
    }

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

    // Making scene objects float
    if (gEnableSceneObjectsFloating) {
      floatSceneObjects(sceneObjects, 0, sceneObjects.size() - 1);
    }

    // Updating flashlight SceneObject fields
    gFlashlightSceneObjectPtr->getTranslate() = gCameraController.getCamera()->getPosition();
    dynamic_cast<SpotLight *>(gFlashlightSceneObjectPtr->getLightPtr().get())
        ->setDirection(gCameraController.getCamera()->getForward());

    // Updating scene objects shader programs uniform values
    SceneObject::updateShadersLights(sceneObjects, glm::vec3{0.1f, 0.1f, 0.1f});
    SceneObject::updateShadersCamera(sceneObjects, gCamera);

    // Enabling Z-testing
    glEnable(GL_DEPTH_TEST);

    // Rendering scene objects
    for (size_t i = 0; i < sceneObjects.size(); ++i) {
      sceneObjects[i].render();
    }

    // Swapping front and back buffers
    glfwSwapBuffers(window);

    // Releasing OpenGL context and mutex
    glfwMakeContextCurrent(nullptr);
    glfwContextLock.unlock();

    std::this_thread::sleep_for(kRenderCycleInterval);
  }

  // Waiting for shaderWatchers to stop
  blinnPhongShaderWatcherIsRunning = false;
  blinnPhongShaderWatcherThread.join();
  lightShaderWatcherIsRunning = false;
  lightShaderWatcherThread.join();

  // Deleting OpenGL objects
  glDeleteProgram(lightSP);
  glDeleteProgram(blinnPhongSP);

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
  // Setting camera aspect ratio
  gCamera.setAspectRatio(static_cast<float>(width) / static_cast<float>(height));
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

  // If cameraController is Controller5DoF
  Controller5DoF *camera5DoFControllerPtr = dynamic_cast<Controller5DoF *>(&gCameraController);
  if (camera5DoFControllerPtr != nullptr) {
    // Rotating camera
    camera5DoFControllerPtr->addAngles(glm::radians(offsetX), glm::radians(offsetY));
  }

  // If cameraController is Controller6DoF
  Controller6DoF *camera6DoFControllerPtr = dynamic_cast<Controller6DoF *>(&gCameraController);
  if (camera6DoFControllerPtr != nullptr) {
    // Rotating camera
    camera6DoFControllerPtr->rotateRight(glm::radians(offsetY));
    camera6DoFControllerPtr->rotateUp(glm::radians(offsetX));
  }
}

void scrollCallback(GLFWwindow *window, double offsetX, double offsetY) {
  // Checking if camera is orthographic
  OrthographicCamera *orthoCameraPtr = dynamic_cast<OrthographicCamera *>(&gCamera);
  if (orthoCameraPtr != nullptr) {
    // Getting orthographic projection attributes of camera
    float leftBorder   = orthoCameraPtr->getLeftBorder();
    float rightBorder  = orthoCameraPtr->getRightBorder();
    float bottomBorder = orthoCameraPtr->getBottomBorder();
    float topBorder    = orthoCameraPtr->getTopBorder();

    // Updating border attributes
    leftBorder -= static_cast<float>(glm::radians(offsetY));
    rightBorder += static_cast<float>(glm::radians(offsetY));
    bottomBorder -= static_cast<float>(glm::radians(offsetY));
    topBorder += static_cast<float>(glm::radians(offsetY));

    // Setting orthographic projection attributes of camera
    orthoCameraPtr->setLeftBorder(leftBorder);
    orthoCameraPtr->setRightBorder(rightBorder);
    orthoCameraPtr->setBottomBorder(bottomBorder);
    orthoCameraPtr->setTopBorder(topBorder);
  }

  // Checking if camera is perspective
  PerspectiveCamera *perspCameraPtr = dynamic_cast<PerspectiveCamera *>(&gCamera);
  if (perspCameraPtr != nullptr) {
    // Getting perspective projection attributes of camera
    float verticalVOF = perspCameraPtr->getVerticalFOV();

    // Updating verticalFOV attribute
    verticalVOF -= static_cast<float>(glm::radians(offsetY));
    if (verticalVOF < glm::radians(1.0f)) {
      verticalVOF = glm::radians(1.0f);
    } else if (verticalVOF > glm::radians(179.0f)) {
      verticalVOF = glm::radians(179.0f);
    }

    // Setting perspective projection attributes of camera
    perspCameraPtr->setVerticalFOV(verticalVOF);
  }
}

void processUserInput(GLFWwindow *window) {
  static bool sPressed{};
  bool        released{true};

  // Processing movement
  float distance = kCameraVelocity * gDeltaTime;
  if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
    distance *= kCameraSprintCoef;
  }
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

  // If cameraController is Controller6DoF
  Controller6DoF *camera6DoFController = dynamic_cast<Controller6DoF *>(&gCameraController);
  // Processing movement
  if (camera6DoFController != nullptr) {
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
      camera6DoFController->rotateForward(-distance);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
      camera6DoFController->rotateForward(distance);
    }
  }

  // Toggling flashlight
  if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      gFlashlightSceneObjectPtr->getLightPtr()->getColor() *= -1.0f;
    }
  }

  // Toggling scene objects floating
  if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      gEnableSceneObjectsFloating = !gEnableSceneObjectsFloating;
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

void floatSceneObjects(std::vector<SceneObject> &sceneObjects, unsigned int startIndex,
                       unsigned int count) {
  static std::vector<SceneObject *> sSceneObjectPtrs{};
  static std::vector<glm::vec3>     sInitialTranslations{};
  static std::vector<float>         sTranslationAmplitudes{};
  static std::vector<float>         sTranslationFrequencies{};
  static std::vector<glm::vec3>     sInitialRotations{};
  static std::vector<float>         sRotationAmplitudes{};
  static std::vector<float>         sRotationFrequencies{};
  static std::vector<glm::vec3>     sInitialScalings{};
  static std::vector<float>         sCalingAmplitudes{};
  static std::vector<float>         sCalingFrequencies{};

  constexpr float kMaxTranslationAmplitude{0.1f};
  constexpr float kMaxTranslationFrequency{1.0f};
  constexpr float kMaxRotationAmplitude{30.0f};
  constexpr float kMaxRotationFrequency{0.5f};
  constexpr float kMaxScalingAmplitude{0.02f};
  constexpr float kMaxScalingFrequency{0.2f};

  // Initializing static variables
  if (sSceneObjectPtrs.size() == 0) {
    srand(time(0));

    for (unsigned int i = startIndex; i < startIndex + count; ++i) {
      sSceneObjectPtrs.push_back(&sceneObjects[i]);
      sInitialTranslations.push_back(sceneObjects[i].getTranslate());
      sInitialRotations.push_back(sceneObjects[i].getRotate());
      sInitialScalings.push_back(sceneObjects[i].getScale());

      sTranslationAmplitudes.push_back(kMaxTranslationAmplitude * static_cast<float>(rand()) /
                                       static_cast<float>(RAND_MAX));
      sTranslationFrequencies.push_back(kMaxTranslationFrequency * static_cast<float>(rand()) /
                                        static_cast<float>(RAND_MAX));
      sRotationAmplitudes.push_back(kMaxRotationAmplitude * static_cast<float>(rand()) /
                                    static_cast<float>(RAND_MAX));
      sRotationFrequencies.push_back(kMaxRotationFrequency * static_cast<float>(rand()) /
                                     static_cast<float>(RAND_MAX));
      sCalingAmplitudes.push_back(kMaxScalingAmplitude * static_cast<float>(rand()) /
                                  static_cast<float>(RAND_MAX));
      sCalingFrequencies.push_back(kMaxScalingFrequency * static_cast<float>(rand()) /
                                   static_cast<float>(RAND_MAX));
    }
  }

  // For each scene object
  for (size_t i = 0; i < sSceneObjectPtrs.size(); ++i) {
    sSceneObjectPtrs[i]->getTranslate() =
        sInitialTranslations[i] +
        glm::vec3(0.0f, 1.0f, 0.0f) * sTranslationAmplitudes[i] *
            std::sin(static_cast<float>(glfwGetTime()) * sTranslationFrequencies[i]);
    sSceneObjectPtrs[i]->getRotate() =
        sInitialRotations[i] +
        glm::vec3(0.0f, 1.0f, 0.0f) * sRotationAmplitudes[i] *
            std::sin(static_cast<float>(glfwGetTime()) * sRotationFrequencies[i]);
    sSceneObjectPtrs[i]->getScale() =
        sInitialScalings[i] *
        (1.0f + sCalingAmplitudes[i] *
                    std::sin(static_cast<float>(glfwGetTime()) * sCalingFrequencies[i]));
  }
}
