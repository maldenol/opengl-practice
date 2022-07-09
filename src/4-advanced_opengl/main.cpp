// STD
#include <atomic>
#include <chrono>
#include <cstdlib>
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
#include <glm/gtc/random.hpp>
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
static constexpr unsigned int          kOutlineMeshIndex    = 2;
static constexpr unsigned int          kInstancingMeshIndex = 3;
static constexpr unsigned int          kInstanceCount       = 1000;
static constexpr float                 kInstanceMaxDistance = 10.0f;
static constexpr float                 kInstanceMaxScale    = 5.0f;

// Global variables
float                gCurrTime{};
float                gDeltaTime{};
PerspectiveCamera    gCamera{};
Camera6DoFController gCameraController{&gCamera};
SceneObject         *gFlashlightSceneObjectPtr{};
SceneObject         *gSkyboxSceneObjectPtr{};
int                  gPolygonMode{};
bool                 gEnableSceneObjectsFloating{true};
bool                 gEnablePostprocessing{false};
bool                 gEnableNormals{false};

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

  // Setting pseudo random generator seed
  srand(glfwGetTime());

  // Creating and binding postprocessing framebuffer
  GLuint postprocessingFBO = 0;
  glGenFramebuffers(1, &postprocessingFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, postprocessingFBO);
  // Creating and binding texture for postprocessing framebuffer
  GLuint postprocessingTexture = 0;
  glGenTextures(1, &postprocessingTexture);
  glBindTexture(GL_TEXTURE_2D, postprocessingTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, kWidth, kHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postprocessingTexture,
                         0);
  // Creating and binding renderbuffer for postprocessing framebuffer
  GLuint postprocessingRBO = 0;
  glGenRenderbuffers(1, &postprocessingRBO);
  glBindRenderbuffer(GL_RENDERBUFFER, postprocessingRBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, kWidth, kHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                            postprocessingRBO);
  // Checking if postprocessing framebuffer is complete and unbinding it
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &postprocessingFBO);
    glDeleteTextures(1, &postprocessingTexture);
    glDeleteRenderbuffers(1, &postprocessingRBO);

    std::cout << "Postprocessing framebuffer object is incomplete!" << std::endl;

    return -1;
  }
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // Creating and binding multisampling framebuffer
  GLuint multisamplingFBO = 0;
  glGenFramebuffers(1, &multisamplingFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, multisamplingFBO);
  // Creating and binding texture for multisampling framebuffer
  GLuint multisamplingTexture = 0;
  glGenTextures(1, &multisamplingTexture);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multisamplingTexture);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, kWidth, kHeight, GL_TRUE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE,
                         multisamplingTexture, 0);
  // Creating and binding renderbuffer for multisampling framebuffer
  GLuint multisamplingRBO = 0;
  glGenRenderbuffers(1, &multisamplingRBO);
  glBindRenderbuffer(GL_RENDERBUFFER, multisamplingRBO);
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, kWidth, kHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                            multisamplingRBO);
  // Checking if multisampling framebuffer is complete and unbinding it
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &multisamplingFBO);
    glDeleteTextures(1, &multisamplingTexture);
    glDeleteRenderbuffers(1, &multisamplingRBO);

    std::cout << "Multisampling framebuffer object is incomplete!" << std::endl;

    return -1;
  }
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Creating vectors of vectors of types of shaders
  std::vector<std::vector<GLuint>> shaderTypes{
      std::vector<GLuint>{
                          GL_VERTEX_SHADER, GL_FRAGMENT_SHADER,                     },
      std::vector<GLuint>{
                          GL_VERTEX_SHADER,  GL_GEOMETRY_SHADER,               GL_FRAGMENT_SHADER, },
  };
  // Creating vectors of filenames of shaders
  std::vector<QString> blinnPhongShaderFilenames{
      getAbsolutePathRelativeToExecutable("blinnPhongVS.glsl"),
      getAbsolutePathRelativeToExecutable("blinnPhongFS.glsl"),
  };
  std::vector<QString> lightShaderFilenames{
      getAbsolutePathRelativeToExecutable("lightVS.glsl"),
      getAbsolutePathRelativeToExecutable("lightFS.glsl"),
  };
  std::vector<QString> screenShaderFilenames{
      getAbsolutePathRelativeToExecutable("screenVS.glsl"),
      getAbsolutePathRelativeToExecutable("screenFS.glsl"),
  };
  std::vector<QString> normalShaderFilenames{
      getAbsolutePathRelativeToExecutable("normalVS.glsl"),
      getAbsolutePathRelativeToExecutable("normalGS.glsl"),
      getAbsolutePathRelativeToExecutable("normalFS.glsl"),
  };
  std::vector<QString> instanceShaderFilenames{
      getAbsolutePathRelativeToExecutable("instanceVS.glsl"),
      getAbsolutePathRelativeToExecutable("blinnPhongFS.glsl"),
  };
  std::vector<QString> skyboxShaderFilenames{
      getAbsolutePathRelativeToExecutable("skyboxVS.glsl"),
      getAbsolutePathRelativeToExecutable("skyboxFS.glsl"),
  };
  // Creating shader programs
  GLuint blinnPhongSP = glCreateProgram();
  GLuint lightSP      = glCreateProgram();
  GLuint screenSP     = glCreateProgram();
  GLuint normalSP     = glCreateProgram();
  GLuint instanceSP   = glCreateProgram();
  GLuint skyboxSP     = glCreateProgram();
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
                                            std::cref(shaderTypes[0]),
                                            std::cref(blinnPhongShaderFilenames)};
  std::atomic<bool> lightShaderWatcherIsRunning = true;
  std::atomic<bool> lightShadersAreRecompiled   = false;
  std::thread       lightShaderWatcherThread{shaderWatcher,
                                       std::cref(lightShaderWatcherIsRunning),
                                       std::ref(lightShadersAreRecompiled),
                                       window,
                                       std::ref(glfwContextMutex),
                                       lightSP,
                                       std::cref(shaderTypes[0]),
                                       std::cref(lightShaderFilenames)};
  std::atomic<bool> screenShaderWatcherIsRunning = true;
  std::atomic<bool> screenShadersAreRecompiled   = false;
  std::thread       screenShaderWatcherThread{shaderWatcher,
                                        std::cref(screenShaderWatcherIsRunning),
                                        std::ref(screenShadersAreRecompiled),
                                        window,
                                        std::ref(glfwContextMutex),
                                        screenSP,
                                        std::cref(shaderTypes[0]),
                                        std::cref(screenShaderFilenames)};
  std::atomic<bool> normalShaderWatcherIsRunning = true;
  std::atomic<bool> normalShadersAreRecompiled   = false;
  std::thread       normalShaderWatcherThread{shaderWatcher,
                                        std::cref(normalShaderWatcherIsRunning),
                                        std::ref(normalShadersAreRecompiled),
                                        window,
                                        std::ref(glfwContextMutex),
                                        normalSP,
                                        std::cref(shaderTypes[1]),
                                        std::cref(normalShaderFilenames)};
  std::atomic<bool> instanceShaderWatcherIsRunning = true;
  std::atomic<bool> instanceShadersAreRecompiled   = false;
  std::thread       instanceShaderWatcherThread{shaderWatcher,
                                          std::cref(instanceShaderWatcherIsRunning),
                                          std::ref(instanceShadersAreRecompiled),
                                          window,
                                          std::ref(glfwContextMutex),
                                          instanceSP,
                                          std::cref(shaderTypes[0]),
                                          std::cref(instanceShaderFilenames)};
  std::atomic<bool> skyboxShaderWatcherIsRunning = true;
  std::atomic<bool> skyboxShadersAreRecompiled   = false;
  std::thread       skyboxShaderWatcherThread{shaderWatcher,
                                        std::cref(skyboxShaderWatcherIsRunning),
                                        std::ref(skyboxShadersAreRecompiled),
                                        window,
                                        std::ref(glfwContextMutex),
                                        skyboxSP,
                                        std::cref(shaderTypes[0]),
                                        std::cref(skyboxShaderFilenames)};

  // Loading textures
  std::vector<std::vector<Mesh::Material::Texture>> textures{
      std::vector<Mesh::Material::Texture>{},
      std::vector<Mesh::Material::Texture>{
                                           Mesh::Material::Texture{0, loadTexture("albedoMap.png")},Mesh::Material::Texture{1, loadTexture("normalMap.png")},
                                           Mesh::Material::Texture{2, loadTexture("heightMap.png")},
                                           Mesh::Material::Texture{3, loadTexture("ambientOcclusionMap.png")},
                                           Mesh::Material::Texture{4, loadTexture("roughnessMap.png")},
                                           //Mesh::Material::Texture{5, loadTexture("emissionMap.png")},
      },
      std::vector<Mesh::Material::Texture>{
                                           Mesh::Material::Texture{0, loadTexture("cubemap.png")},}
  };

  // Creating and configuring scene objects
  std::vector<SceneObject> sceneObjects{};
  // Lower plane
  sceneObjects.push_back(SceneObject{
      glm::vec3{   0.0f, -1.0f, 0.0f},
      glm::vec3{  90.0f,  0.0f, 0.0f},
      glm::vec3{  20.0f, 10.0f, 30.0f},
      std::shared_ptr<BaseLight>{nullptr      },
      std::make_shared<Mesh>(generatePlane(1.0f, 10, blinnPhongSP, textures[1]))
  });
  sceneObjects[sceneObjects.size() - 1].getMeshPtr()->getMaterial().maxHeight = 0.5f;
  // Upper plane
  sceneObjects.push_back(SceneObject{
      glm::vec3{   0.0f,   2.0f, 0.0f},
      glm::vec3{  90.0f, 180.0f, 0.0f},
      glm::vec3{  20.0f,  10.0f, 30.0f},
      std::shared_ptr<BaseLight>{nullptr       },
      std::make_shared<Mesh>(generatePlane(1.0f, 10, blinnPhongSP, textures[1]))
  });
  sceneObjects[sceneObjects.size() - 1].getMeshPtr()->getMaterial().glossiness = 5.0f;
  sceneObjects[sceneObjects.size() - 1].getMeshPtr()->getMaterial().maxHeight  = 0.5f;
  // Central cube
  sceneObjects.push_back(SceneObject{
      glm::vec3{   0.1f,   0.1f, 0.1f},
      glm::vec3{ 180.0f, 180.0f, 180.0f},
      glm::vec3{   2.0f,   2.0f, 2.0f},
      std::shared_ptr<BaseLight>{nullptr       },
      std::make_shared<Mesh>(generateCube(0.5f, 10, false, blinnPhongSP, textures[1]))
  });
  sceneObjects[sceneObjects.size() - 1].getMeshPtr()->getMaterial().glossiness = 10.0f;
  // Instance cube
  sceneObjects.push_back(SceneObject{
      glm::vec3{   0.0f, 0.0f, 0.0f},
      glm::vec3{   0.0f, 0.0f, 0.0f},
      glm::vec3{   1.0f, 1.0f, 1.0f},
      std::shared_ptr<BaseLight>{nullptr     },
      std::make_shared<Mesh>(generateCube(0.5f, 10, false, instanceSP, textures[1]))
  });
  sceneObjects[sceneObjects.size() - 1].getMeshPtr()->getMaterial().glossiness = 10.0f;
  // Directional light (white)
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
  // Point light (purple)
  sceneObjects.push_back(SceneObject{
      glm::vec3{0.0f, 0.8f, -1.0f},
      glm::vec3{0.0f, 0.0f,  0.0f},
      glm::vec3{1.0f, 1.0f,  1.0f},
      std::make_shared<PointLight>(glm::vec3{1.0f, 0.0f,  1.0f},
      1.0f, 0.45f, 0.075),
      std::make_shared<Mesh>(generateQuadSphere(0.1f, 10, true, lightSP, textures[0]))
  });
  // Spot light (green)
  sceneObjects.push_back(SceneObject{
      glm::vec3{-0.1f, 0.75f, -0.1f},
      glm::vec3{ 0.0f, 90.0f,  0.0f},
      glm::vec3{ 1.0f,  1.0f,  1.0f},
      std::make_shared<SpotLight>(glm::vec3{ 0.0f,  1.0f,  0.0f},
      1.0f, glm::vec3{ 0.6f, -1.0f,  0.9f},
                                  0.45f, 0.075, 15.0f, 13.0f),
      std::make_shared<Mesh>(generateUVSphere(0.1f, 10, lightSP, textures[0]))
  });
  // Spot light (yellow)
  sceneObjects.push_back(SceneObject{
      glm::vec3{0.1f,  1.0f, 0.1f},
      glm::vec3{0.0f,  0.0f, 0.0f},
      glm::vec3{1.0f,  1.0f, 1.0f},
      std::make_shared<SpotLight>(glm::vec3{1.0f,  1.0f, 0.0f},
      1.0f, glm::vec3{0.3f, -1.0f, 0.6f},
                                  0.45f, 0.075, 30.0f, 25.0f),
      std::make_shared<Mesh>(generateIcoSphere(0.1f, lightSP, textures[0]))
  });
  // Flashlight
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
  // Skybox
  sceneObjects.push_back(SceneObject{
      glm::vec3{   0.0f, 0.0f, 0.0f},
      glm::vec3{   0.0f, 0.0f, 0.0f},
      glm::vec3{   1.0f, 1.0f, 1.0f},
      std::shared_ptr<BaseLight>{nullptr     },
      std::make_shared<Mesh>(generateCube(1.0f, 1, true, skyboxSP, textures[2]))
  });
  gSkyboxSceneObjectPtr = &sceneObjects[sceneObjects.size() - 1];

  // Creating screen
  float screenVertices[] = {
      -1.0f, 1.0f,  0.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
      -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,
  };
  GLuint screenIndices[] = {
      0, 2, 1, 1, 2, 3,
  };
  GLuint screenVAO = 0, screenVBO = 0, screenEBO = 0;
  glGenVertexArrays(1, &screenVAO);
  glGenBuffers(1, &screenVBO);
  glGenBuffers(1, &screenEBO);
  glBindVertexArray(screenVAO);
  glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), &screenVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void *>(0));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        reinterpret_cast<void *>(2 * sizeof(float)));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screenEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(screenIndices), &screenIndices, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Generating instancing model matrices
  glm::mat4 modelMatrices[kInstanceCount];
  for (unsigned int i = 0; i < kInstanceCount; ++i) {
    modelMatrices[i] = glm::mat4{1.0f};

    modelMatrices[i] = glm::translate(modelMatrices[i], glm::sphericalRand(kInstanceMaxDistance));
    modelMatrices[i] = glm::rotate(
        modelMatrices[i],
        glm::radians(360.0f * static_cast<float>(rand()) / static_cast<float>(RAND_MAX)),
        glm::sphericalRand(1.0f));
    modelMatrices[i] = glm::scale(
        modelMatrices[i],
        kInstanceMaxScale * glm::vec3{static_cast<float>(rand()) / static_cast<float>(RAND_MAX)});
  }
  GLuint instanceVBO;
  glGenBuffers(1, &instanceVBO);
  glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(modelMatrices), &modelMatrices[0], GL_STATIC_DRAW);
  glBindVertexArray(sceneObjects[kInstancingMeshIndex].getMeshPtr()->getVAO());
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float),
                        reinterpret_cast<void *>(0 * sizeof(float)));
  glVertexAttribDivisor(4, 1);
  glEnableVertexAttribArray(5);
  glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float),
                        reinterpret_cast<void *>(4 * sizeof(float)));
  glVertexAttribDivisor(5, 1);
  glEnableVertexAttribArray(6);
  glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float),
                        reinterpret_cast<void *>(8 * sizeof(float)));
  glVertexAttribDivisor(6, 1);
  glEnableVertexAttribArray(7);
  glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float),
                        reinterpret_cast<void *>(12 * sizeof(float)));
  glVertexAttribDivisor(7, 1);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Setting OpenGL clear color
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  // Enabling culling
  glEnable(GL_CULL_FACE);
  // Enabling MSAA
  glEnable(GL_MULTISAMPLE);

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
      // Deleting postprocessing and multisampling framebuffers
      glDeleteFramebuffers(1, &postprocessingFBO);
      glDeleteTextures(1, &postprocessingTexture);
      glDeleteRenderbuffers(1, &postprocessingRBO);
      glDeleteFramebuffers(1, &multisamplingFBO);
      glDeleteTextures(1, &multisamplingTexture);
      glDeleteRenderbuffers(1, &multisamplingRBO);

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

    // If blinnPhong shaders are recompiled
    if (blinnPhongShadersAreRecompiled) {
      // Setting uniform values
      glUseProgram(blinnPhongSP);
      glUniform3fv(glGetUniformLocation(blinnPhongSP, "AMBIENT_LIGHT.color"), 1,
                   glm::value_ptr(glm::vec3{1.0f, 1.0f, 1.0f}));
      glUniform1f(glGetUniformLocation(blinnPhongSP, "AMBIENT_LIGHT.intensity"), 1.0f);
      glUseProgram(0);

      // Notifying that all routine after blinnPhong shader recompilation is done
      blinnPhongShadersAreRecompiled = false;
    }

    // Making scene objects float
    if (gEnableSceneObjectsFloating) {
      floatSceneObjects(sceneObjects, 0, sceneObjects.size() - 1);
    }

    // Updating flashlight SceneObjcet fields
    gFlashlightSceneObjectPtr->setTranslate(gCameraController.getCamera()->getPosition());
    dynamic_cast<SpotLight *>(gFlashlightSceneObjectPtr->getLightPtr().get())
        ->setDirection(gCameraController.getCamera()->getForwardDirection());

    // Updating skybox SceneObject fields
    gSkyboxSceneObjectPtr->setTranslate(gCameraController.getCamera()->getPosition());

    // If postprocessing is enabled
    if (gEnablePostprocessing) {
      // Binding multisampling framebuffer
      glBindFramebuffer(GL_FRAMEBUFFER, multisamplingFBO);
    }

    // Enabling Z- and stencil testing
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    // Enabling blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Clearing color, depth and stencil buffers
    glStencilMask(0xff);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Configuring stencil testing
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xff);
    glStencilMask(0x00);
    // Rendering scene objects
    for (unsigned int i = 0; i < sceneObjects.size(); ++i) {
      if (&sceneObjects[i] == gSkyboxSceneObjectPtr) {
        glCullFace(GL_FRONT);
        glDepthFunc(GL_LEQUAL);
      }

      if (i == kOutlineMeshIndex) {
        glStencilMask(0xff);
      }

      if (i == kInstancingMeshIndex) {
        sceneObjects[i].render(gCamera, sceneObjects, kInstanceCount);
      } else {
        sceneObjects[i].render(gCamera, sceneObjects);

        if (gEnableNormals && sceneObjects[i].getMeshPtr() != nullptr) {
          GLuint initShaderProgram = sceneObjects[i].getMeshPtr()->getShaderProgram();
          sceneObjects[i].getMeshPtr()->setShaderProgram(normalSP);
          sceneObjects[i].render(gCamera, std::vector<SceneObject>{});
          sceneObjects[i].getMeshPtr()->setShaderProgram(initShaderProgram);
        }
      }

      if (i == kOutlineMeshIndex) {
        glStencilMask(0x00);
      }

      if (&sceneObjects[i] == gSkyboxSceneObjectPtr) {
        glCullFace(GL_BACK);
        glDepthFunc(GL_LESS);
      }
    }

    // Drawing outline
    {
      // Configuring stencil testing
      glStencilFunc(GL_NOTEQUAL, 1, 0xff);
      glStencilMask(0x00);
      // Disabling Z-testing
      glDisable(GL_DEPTH_TEST);
      // Preparing the mesh
      const glm::vec3 initScale{sceneObjects[kOutlineMeshIndex].getScale()};
      const GLuint    initShaderProgram{
          sceneObjects[kOutlineMeshIndex].getMeshPtr()->getShaderProgram()};
      sceneObjects[kOutlineMeshIndex].setScale(initScale * 1.1f);
      sceneObjects[kOutlineMeshIndex].getMeshPtr()->setShaderProgram(lightSP);
      // Drawing outline
      sceneObjects[kOutlineMeshIndex].render(gCamera, sceneObjects);
      // Reverting mesh changes
      sceneObjects[kOutlineMeshIndex].getMeshPtr()->setShaderProgram(initShaderProgram);
      sceneObjects[kOutlineMeshIndex].setScale(initScale);
    }

    // If postprocessing is enabled
    if (gEnablePostprocessing) {
      // Converting multisampling FBO data to postprocessing one
      glBindFramebuffer(GL_READ_FRAMEBUFFER, multisamplingFBO);
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postprocessingFBO);
      glBlitFramebuffer(0, 0, kWidth, kHeight, 0, 0, kWidth, kHeight, GL_COLOR_BUFFER_BIT,
                        GL_LINEAR);
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
      glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
      // Binding default framebuffer
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      // Clearing color buffer
      glClear(GL_COLOR_BUFFER_BIT);
      // Rendering screen
      glDisable(GL_STENCIL_TEST);
      glDisable(GL_DEPTH_TEST);
      glBindVertexArray(screenVAO);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, postprocessingTexture);
      glUseProgram(screenSP);
      glUniform1i(glGetUniformLocation(screenSP, "texture0"), 0);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
      glUseProgram(0);
      glBindTexture(GL_TEXTURE_2D, 0);
      glBindVertexArray(0);
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

  // Toggling postprocessing
  if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      gEnablePostprocessing = !gEnablePostprocessing;
    }
  }

  // Toggling normals rendering
  if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      gEnableNormals = !gEnableNormals;
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
