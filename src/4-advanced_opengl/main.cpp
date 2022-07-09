// STD
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <ctime>
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
float                gCurrTime{};
float                gDeltaTime{};
PerspectiveCamera    gCamera{};
Camera6DoFController gCameraController{&gCamera};
SceneObject         *gFlashlightSceneObjectPtr{};
int                  gPolygonMode{};
bool                 gEnableSceneObjectsFloating{true};

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
      createWindow(kWidth, kHeight, "triangle", kOpenGLVersionMajor, kOpenGLVersionMinor);

  // Capturing OpenGL context
  glfwMakeContextCurrent(window);

  // Setting callback functions
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSetCursorPosCallback(window, cursorPosCallback);
  glfwSetScrollCallback(window, scrollCallback);

  // Enabling mouse centering
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Creating and binding own framebuffer
  GLuint fbo = 0;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  // Creating and binding texture for own framebuffer
  GLuint texture = 0;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, kWidth, kHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
  // Creating and binding renderbuffer for own framebuffer
  GLuint rbo = 0;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, kWidth, kHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
  // Checking if own framebuffer is complete and unbinding it
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);

    std::cout << "Framebuffer object is incomplete!" << std::endl;

    return -1;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Creating arrays of types of shaders
  std::vector<GLuint> objectShaderTypes{
      GL_VERTEX_SHADER,
      GL_FRAGMENT_SHADER,
  };
  std::vector<GLuint> lightShaderTypes{
      GL_VERTEX_SHADER,
      GL_FRAGMENT_SHADER,
  };
  std::vector<GLuint> screenShaderTypes{
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
      getAbsolutePathRelativeToExecutable("depthFS.glsl"),
  };
  std::vector<QString> screenShaderFilenames{
      getAbsolutePathRelativeToExecutable("screenVS.glsl"),
      getAbsolutePathRelativeToExecutable("screenFS.glsl"),
  };
  // Creating shader programs
  GLuint objectSP = glCreateProgram();
  GLuint lightSP  = glCreateProgram();
  GLuint screenSP = glCreateProgram();
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
  std::atomic<bool> screenShaderWatcherIsRunning = true;
  std::atomic<bool> screenShadersAreRecompiled   = false;
  std::thread       screenShaderWatcherThread{shaderWatcher,
                                       std::cref(screenShaderWatcherIsRunning),
                                       std::ref(screenShadersAreRecompiled),
                                       window,
                                       std::ref(glfwContextMutex),
                                       screenSP,
                                       std::cref(screenShaderTypes),
                                       std::cref(screenShaderFilenames)};

  // Loading textures
  std::vector<std::vector<Mesh::Material::Texture>> textures{
      std::vector<Mesh::Material::Texture>{                                                          },
      std::vector<Mesh::Material::Texture>{
                                           Mesh::Material::Texture{0, loadTexture("albedoMap.png")},Mesh::Material::Texture{1, loadTexture("normalMap.png")},
                                           Mesh::Material::Texture{2, loadTexture("heightMap.png")},
                                           Mesh::Material::Texture{3, loadTexture("ambientOcclusionMap.png")},
                                           Mesh::Material::Texture{4, loadTexture("roughnessMap.png")},
                                           //Mesh::Material::Texture{5, loadTexture("emissionMap.png")},
      },
  };

  // Creating and configuring scene objects
  std::vector<SceneObject> sceneObjects{};
  sceneObjects.push_back(SceneObject{
      glm::vec3{   0.0f, -1.0f, 0.0f},
      glm::vec3{  90.0f,  0.0f, 0.0f},
      glm::vec3{  20.0f, 10.0f, 30.0f},
      std::shared_ptr<BaseLight>{nullptr      },
      std::make_shared<Mesh>(generatePlane(1.0f, 10, objectSP, textures[1]))
  });
  sceneObjects[sceneObjects.size() - 1].getMeshPtr()->getMaterial().maxHeight = 0.5f;
  sceneObjects.push_back(SceneObject{
      glm::vec3{   0.0f,   2.0f, 0.0f},
      glm::vec3{  90.0f, 180.0f, 0.0f},
      glm::vec3{  20.0f,  10.0f, 30.0f},
      std::shared_ptr<BaseLight>{nullptr       },
      std::make_shared<Mesh>(generatePlane(1.0f, 10, objectSP, textures[1]))
  });
  sceneObjects[sceneObjects.size() - 1].getMeshPtr()->getMaterial().glossiness = 5.0f;
  sceneObjects[sceneObjects.size() - 1].getMeshPtr()->getMaterial().maxHeight  = 0.5f;
  sceneObjects.push_back(SceneObject{
      glm::vec3{   0.1f,   0.1f, 0.1f},
      glm::vec3{ 180.0f, 180.0f, 180.0f},
      glm::vec3{   2.0f,   2.0f, 2.0f},
      std::shared_ptr<BaseLight>{nullptr       },
      std::make_shared<Mesh>(generateCube(0.5f, 10, false, objectSP, textures[1]))
  });
  sceneObjects[sceneObjects.size() - 1].getMeshPtr()->getMaterial().glossiness = 10.0f;
  sceneObjects.push_back(SceneObject{
      glm::vec3{   -3.0f,   0.0f, 2.0f},
      glm::vec3{ 0.0f, 0.0f, 0.0f},
      glm::vec3{   2.0f,   2.0f, 2.0f},
      std::shared_ptr<BaseLight>{nullptr       },
      std::make_shared<Mesh>(generateCube(0.5f, 10, false, objectSP, textures[1]))
  });
  sceneObjects[sceneObjects.size() - 1].getMeshPtr()->getMaterial().glossiness = 10.0f;
  sceneObjects.push_back(SceneObject{
      glm::vec3{   0.0f, 10.0f, 0.0f},
      glm::vec3{   0.0f,  0.0f, 0.0f},
      glm::vec3{   1.0f,  1.0f, 1.0f},
      std::make_shared<DirectionalLight>(glm::vec3{   1.0f,  1.0f, 1.0f},
      0.2f,
                                         glm::vec3{   0.5f, -1.0f, 0.0f}
      ),
      std::shared_ptr<Mesh>{nullptr      }
  });
  sceneObjects.push_back(SceneObject{
      glm::vec3{0.0f, 0.8f, -1.0f},
      glm::vec3{0.0f, 0.0f,  0.0f},
      glm::vec3{1.0f, 1.0f,  1.0f},
      std::make_shared<PointLight>(glm::vec3{1.0f, 0.0f,  1.0f},
      1.0f, 0.45f, 0.075),
      std::make_shared<Mesh>(generateQuadSphere(0.1f, 10, true, lightSP, textures[0]))
  });
  sceneObjects.push_back(SceneObject{
      glm::vec3{-0.1f, 0.75f, -0.1f},
      glm::vec3{ 0.0f, 90.0f,  0.0f},
      glm::vec3{ 1.0f,  1.0f,  1.0f},
      std::make_shared<SpotLight>(glm::vec3{ 0.0f,  1.0f,  0.0f},
      1.0f, glm::vec3{ 0.6f, -1.0f,  0.9f},
                                  0.45f, 0.075, 15.0f, 13.0f),
      std::make_shared<Mesh>(generateUVSphere(0.1f, 10, lightSP, textures[0]))
  });
  sceneObjects.push_back(SceneObject{
      glm::vec3{0.1f,  1.0f, 0.1f},
      glm::vec3{0.0f,  0.0f, 0.0f},
      glm::vec3{1.0f,  1.0f, 1.0f},
      std::make_shared<SpotLight>(glm::vec3{1.0f,  1.0f, 0.0f},
      1.0f, glm::vec3{0.3f, -1.0f, 0.6f},
                                  0.45f, 0.075, 30.0f, 25.0f),
      std::make_shared<Mesh>(generateIcoSphere(0.1f, lightSP, textures[0]))
  });
  sceneObjects.push_back(SceneObject{
      glm::vec3{   0.0f, 0.0f, 0.0f},
      glm::vec3{   0.0f, 0.0f, 0.0f},
      glm::vec3{   1.0f, 1.0f, 1.0f},
      std::make_shared<SpotLight>(glm::vec3{   1.0f, 1.0f, 1.0f},
      1.5f, glm::vec3{   0.0f, 0.0f, 0.0f},
                                  0.45f, 0.075, 20.0f, 18.0f),
      std::shared_ptr<Mesh>{nullptr     }
  });
  gFlashlightSceneObjectPtr = &sceneObjects[sceneObjects.size() - 1];

  // Creating screen
  float screenVertices[] = {
      -1.0f,  1.0f,  0.0f, 1.0f,
      1.0f,  1.0f,  1.0f, 1.0f,
      -1.0f, -1.0f,  0.0f, 0.0f,
      1.0f, -1.0f,  1.0f, 0.0f,
  };
  GLuint screenIndices[] = {
    0, 2, 1,
    1, 2, 3,
  };
  GLuint screenVAO = 0, screenVBO = 0, screenEBO = 0;
  glGenVertexArrays(1, &screenVAO);
  glGenBuffers(1, &screenVBO);
  glGenBuffers(1, &screenEBO);
  glBindVertexArray(screenVAO);
  glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), &screenVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screenEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(screenIndices), &screenIndices, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Setting OpenGL clear color
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  // Enabling culling
  glEnable(GL_CULL_FACE);

  // Enabling point size functionality
  glEnable(GL_PROGRAM_POINT_SIZE);
  glPointSize(10.0f);

  // Releasing OpenGL context
  glfwMakeContextCurrent(nullptr);

  // Configuring camera and cameraControllers
  gCamera.setPosition(glm::vec3{0.0f, 1.0f, 2.0f});
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
    if (glfwWindowShouldClose(window)) {
      // Deleting own framebuffer
      glDeleteFramebuffers(1, &fbo);
      glDeleteTextures(1, &texture);
      glDeleteRenderbuffers(1, &rbo);

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

    // If object shaders are recompiled
    if (objectShadersAreRecompiled) {
      // Setting uniform values
      glUseProgram(objectSP);
      glUniform3fv(glGetUniformLocation(objectSP, "AMBIENT_LIGHT.color"), 1,
                   glm::value_ptr(glm::vec3{1.0f, 1.0f, 1.0f}));
      glUniform1f(glGetUniformLocation(objectSP, "AMBIENT_LIGHT.intensity"), 1.0f);
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

    // Making scene objects float
    if (gEnableSceneObjectsFloating) {
      floatSceneObjects(sceneObjects, 0, sceneObjects.size() - 1);
    }

    // Updating flashlight SceneObjcet fields
    gFlashlightSceneObjectPtr->getTranslate() = gCameraController.getCamera()->getPosition();
    dynamic_cast<SpotLight *>(gFlashlightSceneObjectPtr->getLightPtr().get())
        ->setDirection(gCameraController.getCamera()->getForwardDirection());

    // // Bind own framebuffer
    // glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Enabling Z- and stencil testing
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    // Enabling blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Clearing color, depth and stencil buffers
    glStencilMask(0xff);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Mesh to outline
    constexpr unsigned int kOutlineMeshIndex = 2;
    // Configuring stencil testing
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xff);
    // Rendering scene objects
    for (unsigned int i = 0; i < sceneObjects.size(); ++i) {
      if (i == kOutlineMeshIndex) {
        glStencilMask(0xff);
      } else {
        glStencilMask(0x00);
      }

      sceneObjects[i].render(gCamera, sceneObjects);
    }
    // Configuring stencil testing
    glStencilFunc(GL_NOTEQUAL, 1, 0xff);
    glStencilMask(0x00);
    // Disabling Z-testing
    glDisable(GL_DEPTH_TEST);
    // Preparing the mesh
    const glm::vec3 initScale{sceneObjects[kOutlineMeshIndex].getScale()};
    const GLuint initShaderProgram{sceneObjects[kOutlineMeshIndex].getMeshPtr()->getShaderProgram()};
    sceneObjects[kOutlineMeshIndex].setScale(initScale * 1.1f);
    sceneObjects[kOutlineMeshIndex].getMeshPtr()->setShaderProgram(lightSP);
    // Drawing outline
    sceneObjects[kOutlineMeshIndex].render(gCamera, sceneObjects);
    // Reverting mesh changes
    sceneObjects[kOutlineMeshIndex].getMeshPtr()->setShaderProgram(initShaderProgram);
    sceneObjects[kOutlineMeshIndex].setScale(initScale);

    // // Binding default framebuffer
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // // Clearing color buffer
    // glClear(GL_COLOR_BUFFER_BIT);
    // // Rendering screen
    // glDisable(GL_STENCIL_TEST);
    // glDisable(GL_DEPTH_TEST);
    // glBindVertexArray(screenVAO);
    // glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, texture);
    // glUseProgram(screenSP);
    // glUniform1i(glGetUniformLocation(screenSP, "texture0"), 0);
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    // glUseProgram(0);
    // glBindTexture(GL_TEXTURE_2D, 0);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);

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

  // Toggling flashlight
  if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      gFlashlightSceneObjectPtr->getLightPtr()->getIntensity() *= -1.0f;
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
          break;
        case 1:
          glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
          break;
        case 2:
          glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
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
  for (unsigned int i = 0; i < sSceneObjectPtrs.size(); ++i) {
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
