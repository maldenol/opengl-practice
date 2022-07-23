// STD
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <ctime>
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
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

// "glengine" internal library
#include <glengine.hpp>

using namespace glengine;

// for "ms"
using namespace std::chrono_literals;

// Global constants
static constexpr unsigned int          kInitWidth           = 800;
static constexpr unsigned int          kInitHeight          = 600;
static constexpr unsigned int          kMSAASampleCount     = 4;
static constexpr int                   kOpenGLVersionMajor  = 4;
static constexpr int                   kOpenGLVersionMinor  = 6;
static constexpr std::chrono::duration kRenderCycleInterval = 16ms;
static constexpr float                 kCameraVelocity      = 1.0f;
static constexpr float                 kCameraSprintCoef    = 3.0f;
static constexpr unsigned int          kOutlineMeshIndex    = 2;
static constexpr unsigned int          kInstancedMeshIndex  = 3;
static constexpr unsigned int          kInstanceCount       = 100;
static constexpr float                 kInstanceMaxDistance = 15.0f;
static constexpr float                 kInstanceMaxScale    = 5.0f;

// Global variables
unsigned int      gWidth{kInitWidth};
unsigned int      gHeight{kInitHeight};
float             gCurrTime{};
float             gDeltaTime{};
PerspectiveCamera gCamera{};
Controller6DoF    gCameraController{&gCamera};
SceneObject      *gFlashlightSceneObjectPtr{};
int               gPolygonMode{};
bool              gEnableSceneObjectsFloating{true};
bool              gEnablePostprocessing{false};
bool              gEnableNormals{false};
GLuint            gPostprocessingFBO{};
GLuint            gPostprocessingTexture{};
GLuint            gPostprocessingRBO{};
GLuint            gMultisamplingFBO{};
GLuint            gMultisamplingTexture{};
GLuint            gMultisamplingRBO{};

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
      createWindow(gWidth, gHeight, "4-advanced_opengl", kOpenGLVersionMajor, kOpenGLVersionMinor);

  // Capturing OpenGL context
  glfwMakeContextCurrent(window);

  // Setting callback functions
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSetCursorPosCallback(window, cursorPosCallback);
  glfwSetScrollCallback(window, scrollCallback);

  // Enabling mouse centering
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Setting pseudo random generator seed
  srand(time(0));

  // Creating and binding postprocessing framebuffer
  glGenFramebuffers(1, &gPostprocessingFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, gPostprocessingFBO);
  // Creating and binding texture to postprocessing framebuffer
  glGenTextures(1, &gPostprocessingTexture);
  glBindTexture(GL_TEXTURE_2D, gPostprocessingTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, gWidth, gHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         gPostprocessingTexture, 0);
  // Creating and binding renderbuffer to postprocessing framebuffer
  glGenRenderbuffers(1, &gPostprocessingRBO);
  glBindRenderbuffer(GL_RENDERBUFFER, gPostprocessingRBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, gWidth, gHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                            gPostprocessingRBO);
  // Checking if postprocessing framebuffer is complete and unbinding it
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &gPostprocessingFBO);
    glDeleteTextures(1, &gPostprocessingTexture);
    glDeleteRenderbuffers(1, &gPostprocessingRBO);

    std::cout << "Postprocessing framebuffer object is incomplete!" << std::endl;

    return -1;
  }
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // Creating and binding multisampling framebuffer
  glGenFramebuffers(1, &gMultisamplingFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, gMultisamplingFBO);
  // Creating and binding texture to multisampling framebuffer
  glGenTextures(1, &gMultisamplingTexture);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gMultisamplingTexture);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, kMSAASampleCount, GL_RGB, gWidth, gHeight,
                          GL_TRUE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE,
                         gMultisamplingTexture, 0);
  // Creating and binding renderbuffer to multisampling framebuffer
  glGenRenderbuffers(1, &gMultisamplingRBO);
  glBindRenderbuffer(GL_RENDERBUFFER, gMultisamplingRBO);
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, kMSAASampleCount, GL_DEPTH24_STENCIL8, gWidth,
                                   gHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                            gMultisamplingRBO);
  // Checking if multisampling framebuffer is complete and unbinding it
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &gMultisamplingFBO);
    glDeleteTextures(1, &gMultisamplingTexture);
    glDeleteRenderbuffers(1, &gMultisamplingRBO);

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
  std::vector<std::string> blinnPhongShaderFilenames{
      getAbsolutePathRelativeToExecutable("blinnPhongVS.glsl"),
      getAbsolutePathRelativeToExecutable("blinnPhongFS.glsl"),
  };
  std::vector<std::string> lightShaderFilenames{
      getAbsolutePathRelativeToExecutable("lightVS.glsl"),
      getAbsolutePathRelativeToExecutable("lightFS.glsl"),
  };
  std::vector<std::string> screenShaderFilenames{
      getAbsolutePathRelativeToExecutable("screenVS.glsl"),
      getAbsolutePathRelativeToExecutable("screenFS.glsl"),
  };
  std::vector<std::string> normalShaderFilenames{
      getAbsolutePathRelativeToExecutable("normalVS.glsl"),
      getAbsolutePathRelativeToExecutable("normalGS.glsl"),
      getAbsolutePathRelativeToExecutable("normalFS.glsl"),
  };
  std::vector<std::string> skyboxShaderFilenames{
      getAbsolutePathRelativeToExecutable("skyboxVS.glsl"),
      getAbsolutePathRelativeToExecutable("skyboxFS.glsl"),
  };
  std::vector<std::string> mirrorShaderFilenames{
      getAbsolutePathRelativeToExecutable("blinnPhongVS.glsl"),
      getAbsolutePathRelativeToExecutable("mirrorFS.glsl"),
  };
  std::vector<std::string> lenseShaderFilenames{
      getAbsolutePathRelativeToExecutable("blinnPhongVS.glsl"),
      getAbsolutePathRelativeToExecutable("lenseFS.glsl"),
  };
  // Creating shader programs
  GLuint blinnPhongSP = glCreateProgram();
  GLuint lightSP      = glCreateProgram();
  GLuint screenSP     = glCreateProgram();
  GLuint normalSP     = glCreateProgram();
  GLuint skyboxSP     = glCreateProgram();
  GLuint mirrorSP     = glCreateProgram();
  GLuint lenseSP      = glCreateProgram();
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
  std::atomic<bool> mirrorShaderWatcherIsRunning = true;
  std::atomic<bool> mirrorShadersAreRecompiled   = false;
  std::thread       mirrorShaderWatcherThread{shaderWatcher,
                                        std::cref(mirrorShaderWatcherIsRunning),
                                        std::ref(mirrorShadersAreRecompiled),
                                        window,
                                        std::ref(glfwContextMutex),
                                        mirrorSP,
                                        std::cref(shaderTypes[0]),
                                        std::cref(mirrorShaderFilenames)};
  std::atomic<bool> lenseShaderWatcherIsRunning = true;
  std::atomic<bool> lenseShadersAreRecompiled   = false;
  std::thread       lenseShaderWatcherThread{shaderWatcher,
                                       std::cref(lenseShaderWatcherIsRunning),
                                       std::ref(lenseShadersAreRecompiled),
                                       window,
                                       std::ref(glfwContextMutex),
                                       lenseSP,
                                       std::cref(shaderTypes[0]),
                                       std::cref(lenseShaderFilenames)};

  // Loading textures
  std::vector<std::vector<std::shared_ptr<Mesh::Material::Texture>>> texturePtrVectors{
      std::vector<std::shared_ptr<Mesh::Material::Texture>>{ },
      std::vector<std::shared_ptr<Mesh::Material::Texture>>{
                                                            std::make_shared<Mesh::Material::Texture>(loadTexture("albedoMap.png", false), 0, false),
                                                            std::make_shared<Mesh::Material::Texture>(loadTexture("normalMap.png", false), 1, false),
                                                            std::make_shared<Mesh::Material::Texture>(loadTexture("depthMap.png", false), 2, false),
                                                            std::make_shared<Mesh::Material::Texture>(loadTexture("ambientOcclusionMap.png", false),
                                                            3, false),
                                                            std::make_shared<Mesh::Material::Texture>(loadTexture("roughnessMap.png", false), 4,
                                                            false),
                                                            //std::make_shared<Mesh::Material::Texture>(loadTexture("emissionMap.png", false), 5, false),
          std::make_shared<Mesh::Material::Texture>(loadCubemap(
                                                        std::vector<std::string>{
                                                            "cubemapXP.png",
                                                            "cubemapXN.png",
                                                            "cubemapYP.png",
                                                            "cubemapYN.png",
                                                            "cubemapZP.png",
                                                            "cubemapZN.png",
                                                        }, false),
                                                            6, true),
                                                            },
      std::vector<std::shared_ptr<Mesh::Material::Texture>>{
                                                            std::make_shared<Mesh::Material::Texture>(loadTexture("cubemap.png", false), 0, false),
                                                            }
  };

  // Creating and configuring scene objects
  std::vector<SceneObject> sceneObjects{};
  // Lower plane
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
  sceneObjects[sceneObjects.size() - 1].getMeshPtr()->getMaterialPtr()->setGlossiness(5.0f);
  sceneObjects[sceneObjects.size() - 1].getMeshPtr()->getMaterialPtr()->setParallaxStrength(0.1f);
  // Central cube
  sceneObjects.push_back(SceneObject{
      glm::vec3{   0.0f, 0.0f, 0.0f},
      glm::vec3{   0.0f, 0.0f, 0.0f},
      glm::vec3{   2.0f, 2.0f, 2.0f},
      std::shared_ptr<BaseLight>{nullptr     },
      std::make_shared<Mesh>(generateCube(0.5f, 10, false, blinnPhongSP, texturePtrVectors[1]))
  });
  sceneObjects[sceneObjects.size() - 1].getMeshPtr()->getMaterialPtr()->setGlossiness(10.0f);
  sceneObjects[sceneObjects.size() - 1].getMeshPtr()->getMaterialPtr()->setParallaxStrength(0.1f);
  // Instance cube
  sceneObjects.push_back(SceneObject{
      glm::vec3{   0.0f, 0.0f, 0.0f},
      glm::vec3{   0.0f, 0.0f, 0.0f},
      glm::vec3{   1.0f, 1.0f, 1.0f},
      std::shared_ptr<BaseLight>{nullptr     },
      std::make_shared<Mesh>(generateCube(0.5f, 10, false, blinnPhongSP, texturePtrVectors[1]))
  });
  sceneObjects[sceneObjects.size() - 1].getMeshPtr()->setInstanceCount(kInstanceCount);
  sceneObjects[sceneObjects.size() - 1].getMeshPtr()->getMaterialPtr()->setGlossiness(15.0f);
  sceneObjects[sceneObjects.size() - 1].getMeshPtr()->getMaterialPtr()->setParallaxStrength(0.1f);
  // Mirror cube
  sceneObjects.push_back(SceneObject{
      glm::vec3{  -3.0f, 0.0f, -10.0f},
      glm::vec3{   0.0f, 0.0f, 0.0f},
      glm::vec3{   3.0f, 3.0f, 3.0f},
      std::shared_ptr<BaseLight>{nullptr     },
      std::make_shared<Mesh>(generateCube(0.5f, 10, false, mirrorSP, texturePtrVectors[1]))
  });
  sceneObjects[sceneObjects.size() - 1].getMeshPtr()->getMaterialPtr()->setParallaxStrength(0.1f);
  // Lense cube
  sceneObjects.push_back(SceneObject{
      glm::vec3{   3.0f, 0.0f, -10.0f},
      glm::vec3{   0.0f, 0.0f, 0.0f},
      glm::vec3{   3.0f, 3.0f, 3.0f},
      std::shared_ptr<BaseLight>{nullptr     },
      std::make_shared<Mesh>(generateCube(0.5f, 10, false, lenseSP, texturePtrVectors[1]))
  });
  sceneObjects[sceneObjects.size() - 1].getMeshPtr()->getMaterialPtr()->setParallaxStrength(0.1f);
  // Directional light (white)
  sceneObjects.push_back(SceneObject{
      glm::vec3{   0.0f,  0.0f, 0.0f},
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
      1.0f, 0.45f, 0.075f),
      std::make_shared<Mesh>(generateQuadSphere(0.1f, 10, true, lightSP, texturePtrVectors[0]))
  });
  // Spot light (green)
  sceneObjects.push_back(SceneObject{
      glm::vec3{-0.1f, 0.75f, -0.1f},
      glm::vec3{ 0.0f, 90.0f,  0.0f},
      glm::vec3{ 1.0f,  1.0f,  1.0f},
      std::make_shared<SpotLight>(glm::vec3{ 0.0f,  1.0f,  0.0f},
      1.0f, glm::vec3{ 0.6f, -1.0f,  0.9f},
                                  0.45f, 0.075f, glm::radians(15.0f), glm::radians(13.0f)),
      std::make_shared<Mesh>(generateUVSphere(0.1f, 10, lightSP, texturePtrVectors[0]))
  });
  // Spot light (yellow)
  sceneObjects.push_back(SceneObject{
      glm::vec3{0.1f,  1.0f, 0.1f},
      glm::vec3{0.0f,  0.0f, 0.0f},
      glm::vec3{1.0f,  1.0f, 1.0f},
      std::make_shared<SpotLight>(glm::vec3{1.0f,  1.0f, 0.0f},
      1.0f, glm::vec3{0.3f, -1.0f, 0.6f},
                                  0.45f, 0.075f, glm::radians(30.0f), glm::radians(25.0f)),
      std::make_shared<Mesh>(generateIcoSphere(0.1f, lightSP, texturePtrVectors[0]))
  });
  // Flashlight
  sceneObjects.push_back(SceneObject{
      glm::vec3{   0.0f, 0.0f, 0.0f},
      glm::vec3{   0.0f, 0.0f, 0.0f},
      glm::vec3{   1.0f, 1.0f, 1.0f},
      std::make_shared<SpotLight>(glm::vec3{   1.0f, 1.0f, 1.0f},
      1.5f, glm::vec3{   0.0f, 0.0f, 0.0f},
                                  0.45f, 0.075f, glm::radians(20.0f), glm::radians(18.0f)),
      std::shared_ptr<Mesh>{nullptr     }
  });
  gFlashlightSceneObjectPtr = &sceneObjects[sceneObjects.size() - 1];

  // Skybox
  SceneObject skyboxSceneObject{
      glm::vec3{   0.0f, 0.0f, 0.0f},
      glm::vec3{   0.0f, 0.0f, 0.0f},
      glm::vec3{   1.0f, 1.0f, 1.0f},
      std::shared_ptr<BaseLight>{nullptr     },
      std::make_shared<Mesh>(generateCube(1.0f, 1, true, skyboxSP, texturePtrVectors[2]))
  };

  // Creating screen
  float screenVertices[] = {
      -1.0f, 1.0f,  0.0f, 1.0f,  // top-left
      1.0f,  1.0f,  1.0f, 1.0f,  // top-right
      -1.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
      1.0f,  -1.0f, 1.0f, 0.0f,  // bottom-right
  };
  GLuint screenIndices[] = {
      0, 2, 1,  // top-right
      1, 2, 3,  // bottom-left
  };
  GLuint screenVAO{}, screenVBO{}, screenEBO{};
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

  // Generating instanced model matrices
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
  glBindVertexArray(sceneObjects[kInstancedMeshIndex].getMeshPtr()->getVAO());
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
  gCamera.setWorldUp(glm::vec3{0.0f, 1.0f, 0.0f});
  gCamera.lookAt(glm::vec3{0.0f, 0.0f, 0.0f});
  gCamera.setVerticalFOV(glm::radians(60.0f));
  gCamera.setAspectRatio(static_cast<float>(gWidth) / static_cast<float>(gHeight));
  gCamera.setNearPlane(0.1f);
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

    // Making scene objects float
    if (gEnableSceneObjectsFloating) {
      floatSceneObjects(sceneObjects, 0, sceneObjects.size() - 1);
    }

    // Updating flashlight SceneObject fields
    gFlashlightSceneObjectPtr->setTranslate(gCameraController.getCamera()->getPosition());
    dynamic_cast<SpotLight *>(gFlashlightSceneObjectPtr->getLightPtr().get())
        ->setDirection(gCameraController.getCamera()->getForward());

    // Updating scene objects shader programs uniform values
    SceneObject::updateShadersLights(sceneObjects);
    SceneObject::updateShadersCamera(sceneObjects, gCamera);

    // If postprocessing is enabled
    if (gEnablePostprocessing) {
      // Binding multisampling framebuffer
      glBindFramebuffer(GL_FRAMEBUFFER, gMultisamplingFBO);
    }

    // Enabling Z- and stencil testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xff);

    // Enabling blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Clearing color, depth and stencil buffers
    glStencilMask(0xff);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glStencilMask(0x00);

    // Rendering scene objects
    for (size_t i = 0; i < sceneObjects.size(); ++i) {
      if (i == kOutlineMeshIndex) {
        glStencilMask(0xff);
      }

      sceneObjects[i].render();

      if (i == kOutlineMeshIndex) {
        glStencilMask(0x00);
      }
    }

    // Drawing outline
    glStencilFunc(GL_NOTEQUAL, 1, 0xff);
    glStencilMask(0x00);
    const glm::vec3 initScale{sceneObjects[kOutlineMeshIndex].getScale()};
    const GLuint    initShaderProgram =
        sceneObjects[kOutlineMeshIndex].getMeshPtr()->getShaderProgram();
    sceneObjects[kOutlineMeshIndex].setScale(initScale * 1.1f);
    sceneObjects[kOutlineMeshIndex].getMeshPtr()->setShaderProgram(lightSP);
    sceneObjects[kOutlineMeshIndex].render();
    sceneObjects[kOutlineMeshIndex].getMeshPtr()->setShaderProgram(initShaderProgram);
    sceneObjects[kOutlineMeshIndex].setScale(initScale);
    glStencilFunc(GL_ALWAYS, 1, 0xff);

    // Rendering normals
    if (gEnableNormals) {
      std::vector<GLuint> initShaderPrograms{};
      initShaderPrograms.resize(sceneObjects.size());

      // Temporary changing scene objects shader programs and updating shaders camera
      // to render normals
      for (size_t i = 0; i < sceneObjects.size(); ++i) {
        if (sceneObjects[i].getMeshPtr() != nullptr) {
          initShaderPrograms[i] = sceneObjects[i].getMeshPtr()->getShaderProgram();
          sceneObjects[i].getMeshPtr()->setShaderProgram(normalSP);
        }
      }
      SceneObject::updateShadersCamera(sceneObjects, gCamera);

      // Rendering normals and revering shader program changes
      for (size_t i = 0; i < sceneObjects.size(); ++i) {
        if (sceneObjects[i].getMeshPtr() != nullptr) {
          sceneObjects[i].render();
          sceneObjects[i].getMeshPtr()->setShaderProgram(initShaderPrograms[i]);
        }
      }
    }

    // Drawing skybox
    glDepthFunc(GL_LEQUAL);
    glCullFace(GL_FRONT);
    skyboxSceneObject.setTranslate(gCameraController.getCamera()->getPosition());
    SceneObject::updateShadersCamera(std::vector<SceneObject>{skyboxSceneObject}, gCamera);
    skyboxSceneObject.render();
    glCullFace(GL_BACK);
    glDepthFunc(GL_LESS);

    // If postprocessing is enabled
    if (gEnablePostprocessing) {
      // Converting multisampling FBO data to postprocessing one
      glBindFramebuffer(GL_READ_FRAMEBUFFER, gMultisamplingFBO);
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gPostprocessingFBO);
      glBlitFramebuffer(0, 0, gWidth, gHeight, 0, 0, gWidth, gHeight, GL_COLOR_BUFFER_BIT,
                        GL_NEAREST);
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
      glBindTexture(GL_TEXTURE_2D, gPostprocessingTexture);
      glUseProgram(screenSP);
      glUniform1i(glGetUniformLocation(screenSP, "texture0"), 0);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
      glUseProgram(0);
      glBindTexture(GL_TEXTURE_2D, 0);
      glBindVertexArray(0);
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_STENCIL_TEST);
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
  glDeleteBuffers(1, &screenEBO);
  glDeleteBuffers(1, &screenVBO);
  glDeleteVertexArrays(1, &screenVAO);
  glDeleteBuffers(1, &instanceVBO);
  glDeleteFramebuffers(1, &gPostprocessingFBO);
  glDeleteTextures(1, &gPostprocessingTexture);
  glDeleteRenderbuffers(1, &gPostprocessingRBO);
  glDeleteFramebuffers(1, &gMultisamplingFBO);
  glDeleteTextures(1, &gMultisamplingTexture);
  glDeleteRenderbuffers(1, &gMultisamplingRBO);

  // Terminating window with OpenGL context and GLFW
  terminateWindow(window);
  terminateGLFW();

  // Terminating Qt Gui application
  terminateQGuiApplication(app);

  return 0;
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  // Updating global variables
  gWidth  = width;
  gHeight = height;
  // Setting viewport position and size relative to window
  glViewport(0, 0, gWidth, gHeight);
  // Setting postprocessing and multisampling framebuffers sizes
  glBindTexture(GL_TEXTURE_2D, gPostprocessingTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, gWidth, gHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, gPostprocessingRBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, gWidth, gHeight);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gMultisamplingTexture);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, kMSAASampleCount, GL_RGB, gWidth, gHeight,
                          GL_TRUE);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, gMultisamplingRBO);
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, kMSAASampleCount, GL_DEPTH24_STENCIL8, gWidth,
                                   gHeight);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  // Setting camera aspect ratio
  gCamera.setAspectRatio(static_cast<float>(gWidth) / static_cast<float>(gHeight));
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
