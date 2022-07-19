// Header file
#include "./SceneObject.hpp"

// STD
#include <algorithm>
#include <utility>

// GLM
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

using namespace glengine;

// Global constants
static constexpr size_t kMaxDirectionLightCount = 8;
static constexpr size_t kMaxPointLightCount     = 8;
static constexpr size_t kMaxSpotLightCount      = 8;

static constexpr int kInitShadowMapTextureUnit = 7;

static constexpr float kShadowMapDistance = 20.0f;

// Local function headers

static void renderSpotLightShadowMap(const std::vector<SceneObject> &sceneObjects,
                                     std::vector<glm::mat4>         &spotLightVPMatrices,
                                     const SceneObject              &spotLightSceneObject,
                                     const SpotLight                *spotLightPtr,
                                     GLuint spotLightShadowMapShaderProgram);
static void renderDirectionalLightShadowMap(const std::vector<SceneObject> &sceneObjects,
                                            std::vector<glm::mat4> &directionalLightVPMatrices,
                                            const SceneObject      &directionalLightSceneObject,
                                            const DirectionalLight *directionalLightPtr,
                                            GLuint           directionalLightShadowMapShaderProgram,
                                            const glm::vec3 &viewPos);
static void renderPointLightShadowMap(const std::vector<SceneObject> &sceneObjects,
                                      std::vector<float>             &pointLightFarPlanes,
                                      const SceneObject              &pointLightSceneObject,
                                      const PointLight               *pointLightPtr,
                                      GLuint pointLightShadowMapShaderProgram);
static void updateShaderProgramSpotLights(
    GLuint shaderProgram, const std::vector<const SceneObject *> &spotLightSceneObjectPtrs,
    GLuint spotLightShadowMapShaderProgram, int &currShadowMapTextureUnit,
    std::vector<glm::mat4> &spotLightVPMatrices);
static void updateShaderProgramDirectionalLights(
    GLuint shaderProgram, const std::vector<const SceneObject *> &directionalLightSceneObjectPtrs,
    GLuint directionalLightShadowMapShaderProgram, int &currShadowMapTextureUnit,
    std::vector<glm::mat4> &directionalLightVPMatrices);
static void updateShaderProgramPointLights(
    GLuint shaderProgram, const std::vector<const SceneObject *> &pointLightSceneObjectPtrs,
    GLuint pointLightShadowMapShaderProgram, int &currShadowMapTextureUnit,
    std::vector<float> &pointLightFarPlanes);

// Constructors, assignment operators and destructor

// Default constructor
SceneObject::SceneObject() noexcept { recalculateModelMatrix(); }

// Parameterized constructor
SceneObject::SceneObject(const glm::vec3 &translate, const glm::vec3 &rotate,
                         const glm::vec3 &scale, const std::shared_ptr<BaseLight> &lightPtr,
                         const std::shared_ptr<Mesh> &meshPtr) noexcept
    : _translate{translate},
      _rotate{rotate},
      _scale{scale},
      _lightPtr{lightPtr},
      _meshPtr{meshPtr} {
  recalculateModelMatrix();
}

// Copy constructor
SceneObject::SceneObject(const SceneObject &sceneObject) noexcept
    : _translate{sceneObject._translate},
      _rotate{sceneObject._rotate},
      _scale{sceneObject._scale},
      _modelMatrix{sceneObject._modelMatrix},
      _lightPtr{sceneObject._lightPtr},
      _meshPtr{sceneObject._meshPtr} {}

// Copy assignment operator
SceneObject &SceneObject::operator=(const SceneObject &sceneObject) noexcept {
  _translate   = sceneObject._translate;
  _rotate      = sceneObject._rotate;
  _scale       = sceneObject._scale;
  _modelMatrix = sceneObject._modelMatrix;
  _lightPtr    = sceneObject._lightPtr;
  _meshPtr     = sceneObject._meshPtr;

  return *this;
}

// Move constructor
SceneObject::SceneObject(SceneObject &&sceneObject) noexcept
    : _translate{std::exchange(sceneObject._translate, glm::vec3{})},
      _rotate{std::exchange(sceneObject._rotate, glm::vec3{})},
      _scale{std::exchange(sceneObject._scale, glm::vec3{})},
      _modelMatrix{std::exchange(sceneObject._modelMatrix, glm::mat4{})},
      _lightPtr{std::exchange(sceneObject._lightPtr, std::shared_ptr<BaseLight>{})},
      _meshPtr{std::exchange(sceneObject._meshPtr, std::shared_ptr<Mesh>{})} {}

// Move assignment operator
SceneObject &SceneObject::operator=(SceneObject &&sceneObject) noexcept {
  std::swap(_translate, sceneObject._translate);
  std::swap(_rotate, sceneObject._rotate);
  std::swap(_scale, sceneObject._scale);
  std::swap(_modelMatrix, sceneObject._modelMatrix);
  std::swap(_lightPtr, sceneObject._lightPtr);
  std::swap(_meshPtr, sceneObject._meshPtr);

  return *this;
}

// Destructor
SceneObject::~SceneObject() noexcept {}

// Setters

void SceneObject::setTranslate(const glm::vec3 &translate) noexcept {
  _translate = translate;
  recalculateModelMatrix();
}

void SceneObject::setRotate(const glm::vec3 &rotate) noexcept {
  _rotate = rotate;
  recalculateModelMatrix();
}

void SceneObject::setScale(const glm::vec3 &scale) noexcept {
  _scale = scale;
  recalculateModelMatrix();
}

void SceneObject::setModelMatrix(const glm::mat4 &modelMatrix) noexcept {
  _modelMatrix = modelMatrix;
}

void SceneObject::setLightPtr(const std::shared_ptr<BaseLight> &lightPtr) noexcept {
  _lightPtr = lightPtr;
}

void SceneObject::setMeshPtr(const std::shared_ptr<Mesh> &meshPtr) noexcept { _meshPtr = meshPtr; }

// Getters

const glm::vec3 &SceneObject::getTranslate() const noexcept { return _translate; }

glm::vec3 &SceneObject::getTranslate() noexcept { return _translate; }

const glm::vec3 &SceneObject::getRotate() const noexcept { return _rotate; }

glm::vec3 &SceneObject::getRotate() noexcept { return _rotate; }

const glm::vec3 &SceneObject::getScale() const noexcept { return _scale; }

glm::vec3 &SceneObject::getScale() noexcept { return _scale; }

const glm::mat4 &SceneObject::getModelMatrix() const noexcept { return _modelMatrix; }

glm::mat4 &SceneObject::getModelMatrix() noexcept { return _modelMatrix; }

const std::shared_ptr<BaseLight> &SceneObject::getLightPtr() const noexcept { return _lightPtr; }

std::shared_ptr<BaseLight> &SceneObject::getLightPtr() noexcept { return _lightPtr; }

const std::shared_ptr<Mesh> &SceneObject::getMeshPtr() const noexcept { return _meshPtr; }

std::shared_ptr<Mesh> &SceneObject::getMeshPtr() noexcept { return _meshPtr; }

// Other member functions

void SceneObject::recalculateModelMatrix() noexcept {
  // Calculating scene object model matrix
  _modelMatrix = glm::translate(glm::mat4{1.0f}, _translate);
  _modelMatrix = _modelMatrix * glm::eulerAngleXYZ(glm::radians(_rotate.x), glm::radians(_rotate.y),
                                                   glm::radians(_rotate.z));
  _modelMatrix = glm::scale(_modelMatrix, _scale);
}

void SceneObject::updateShaderModelMatrix() const noexcept {
  if (_meshPtr != nullptr && _meshPtr->isComplete()) {
    // Updating object shader program uniform values
    const GLuint shaderProgram = _meshPtr->getShaderProgram();

    glUseProgram(shaderProgram);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MODEL"), 1, GL_FALSE,
                       glm::value_ptr(_modelMatrix));

    glUseProgram(0);
  }
}

void SceneObject::render() const noexcept {
  if (_meshPtr != nullptr && _meshPtr->isComplete()) {
    // Updating shader uniform variables
    updateShaderModelMatrix();

    // Rendering mesh
    _meshPtr->render();
  }
}

// Other static member functions

void SceneObject::updateShadersLights(const std::vector<SceneObject> &sceneObjects,
                                      GLuint           directionalLightShadowMapShaderProgram,
                                      GLuint           pointLightShadowMapShaderProgram,
                                      GLuint           spotLightShadowMapShaderProgram,
                                      const glm::vec3 &viewPos) noexcept {
  // Getting shader programs, light sources and light cameras matrices
  std::vector<GLuint>              shaderPrograms{};
  std::vector<const SceneObject *> spotLightSceneObjectPtrs{};
  std::vector<const SceneObject *> directionalLightSceneObjectPtrs{};
  std::vector<const SceneObject *> pointLightSceneObjectPtrs{};
  std::vector<glm::mat4>           spotLightVPMatrices{};
  std::vector<glm::mat4>           directionalLightVPMatrices{};
  std::vector<float>               pointLightFarPlanes{};
  for (size_t i = 0; i < sceneObjects.size(); ++i) {
    const SceneObject &sceneObject = sceneObjects[i];
    const Mesh        *meshPtr     = sceneObject.getMeshPtr().get();
    const BaseLight   *lightPtr    = sceneObject.getLightPtr().get();

    // If scene object has mesh
    if (meshPtr != nullptr && meshPtr->isComplete()) {
      const GLuint shaderProgram = meshPtr->getShaderProgram();

      bool notInVector{true};
      for (size_t i = 0; i < shaderPrograms.size(); ++i) {
        if (shaderProgram == shaderPrograms[i]) {
          notInVector = false;
          break;
        }
      }

      if (notInVector) {
        shaderPrograms.push_back(shaderProgram);
      }
    }

    // Enabling Z- and disabling stencil testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_STENCIL_TEST);

    // Getting original viewport
    int viewport[4] = {0};
    glGetIntegerv(GL_VIEWPORT, &viewport[0]);

    const SpotLight        *spotLightPtr       = dynamic_cast<const SpotLight *>(lightPtr);
    const DirectionalLight *direcionalLightPtr = dynamic_cast<const DirectionalLight *>(lightPtr);
    const PointLight       *pointLightPtr      = dynamic_cast<const PointLight *>(lightPtr);

    // If light is spot (must be before directional and point lights)
    if (spotLightPtr != nullptr) {
      // Adding scene object to spot light vector
      spotLightSceneObjectPtrs.push_back(&sceneObject);

      // Rendering shadow map if shader program is specified
      if (spotLightShadowMapShaderProgram > 0) {
        renderSpotLightShadowMap(sceneObjects, spotLightVPMatrices, sceneObject, spotLightPtr,
                                 spotLightShadowMapShaderProgram);
      }
    }

    // If light is directional
    else if (direcionalLightPtr != nullptr) {
      // Adding scene object to directional light vector
      directionalLightSceneObjectPtrs.push_back(&sceneObject);

      // Rendering shadow map if shader program is specified
      if (directionalLightShadowMapShaderProgram > 0) {
        renderDirectionalLightShadowMap(sceneObjects, directionalLightVPMatrices, sceneObject,
                                        direcionalLightPtr, directionalLightShadowMapShaderProgram,
                                        viewPos);
      }
    }

    // If light is point
    else if (pointLightPtr != nullptr) {
      // Adding scene object to point light vector
      pointLightSceneObjectPtrs.push_back(&sceneObject);

      // Rendering shadow map if shader program is specified
      if (pointLightShadowMapShaderProgram > 0) {
        renderPointLightShadowMap(sceneObjects, pointLightFarPlanes, sceneObject, pointLightPtr,
                                  pointLightShadowMapShaderProgram);
      }
    }

    // Reverting viewport and Z-test changes
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    glDisable(GL_DEPTH_TEST);
  }

  // Updating shader programs uniform values
  int currShadowMapTextureUnit = kInitShadowMapTextureUnit;
  for (size_t i = 0; i < shaderPrograms.size(); ++i) {
    const GLuint shaderProgram = shaderPrograms[i];

    glUseProgram(shaderProgram);

    // Skybox
    glUniform1i(glGetUniformLocation(shaderProgram, "SKYBOX"), 6);

    // Ambient light
    glUniform3fv(glGetUniformLocation(shaderProgram, "AMBIENT_LIGHT.color"), 1,
                 glm::value_ptr(glm::vec3{1.0f, 1.0f, 1.0f}));
    glUniform1f(glGetUniformLocation(shaderProgram, "AMBIENT_LIGHT.intensity"), 1.0f);

    // Copying light VP matrices deques
    std::vector<glm::mat4> spotLightVPMatricesCopy{spotLightVPMatrices.crbegin(),
                                                   spotLightVPMatrices.crend()};
    std::vector<glm::mat4> directionalLightVPMatricesCopy{directionalLightVPMatrices.crbegin(),
                                                          directionalLightVPMatrices.crend()};
    std::vector<float>     pointLightFarPlanesCopy{pointLightFarPlanes.crbegin(),
                                               pointLightFarPlanes.crend()};

    // Spot lights
    updateShaderProgramSpotLights(shaderProgram, spotLightSceneObjectPtrs,
                                  spotLightShadowMapShaderProgram, currShadowMapTextureUnit,
                                  spotLightVPMatricesCopy);

    // Directional lights
    updateShaderProgramDirectionalLights(shaderProgram, directionalLightSceneObjectPtrs,
                                         directionalLightShadowMapShaderProgram,
                                         currShadowMapTextureUnit, directionalLightVPMatricesCopy);

    // Point lights
    updateShaderProgramPointLights(shaderProgram, pointLightSceneObjectPtrs,
                                   pointLightShadowMapShaderProgram, currShadowMapTextureUnit,
                                   pointLightFarPlanesCopy);

    glUseProgram(0);
  }
}

void SceneObject::updateShadersCamera(const std::vector<SceneObject> &sceneObjects,
                                      const BaseCamera               &camera) noexcept {
  // Getting camera view and projection matrices
  const glm::mat4 viewMatrix{camera.getViewMatrix()};
  const glm::mat4 projMatrix{camera.getProjectionMatrix()};
  const glm::vec3 position{camera.getPosition()};

  // For each scene object
  for (size_t i = 0; i < sceneObjects.size(); ++i) {
    const Mesh *meshPtr = sceneObjects[i].getMeshPtr().get();

    // If scene object has mesh
    if (meshPtr != nullptr && meshPtr->isComplete()) {
      // Updating object shader program uniform values
      const GLuint shaderProgram = meshPtr->getShaderProgram();

      glUseProgram(shaderProgram);

      glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "VIEW"), 1, GL_FALSE,
                         glm::value_ptr(viewMatrix));
      glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "PROJ"), 1, GL_FALSE,
                         glm::value_ptr(projMatrix));
      glUniform3fv(glGetUniformLocation(shaderProgram, "VIEW_POS"), 1, glm::value_ptr(position));

      glUseProgram(0);
    }
  }
}

// Local function definition

void renderSpotLightShadowMap(const std::vector<SceneObject> &sceneObjects,
                              std::vector<glm::mat4>         &spotLightVPMatrices,
                              const SceneObject              &spotLightSceneObject,
                              const SpotLight                *spotLightPtr,
                              GLuint                          spotLightShadowMapShaderProgram) {
  // Setting shadow map viewport
  glViewport(0, 0, spotLightPtr->getShadowMapTextureResolution(),
             spotLightPtr->getShadowMapTextureResolution());

  // Binding and clearing shadow map framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, spotLightPtr->getShadowMapFBO());
  glClear(GL_DEPTH_BUFFER_BIT);

  // Configuring camera viewing from light position in light direction
  PerspectiveCamera shadowMapCamera{};
  shadowMapCamera.setPosition(spotLightSceneObject.getTranslate());
  shadowMapCamera.setWorldUp(kUp);
  shadowMapCamera.look(glm::vec3{
      glm::eulerAngleXYZ(glm::radians(spotLightSceneObject.getRotate().x),
                         glm::radians(spotLightSceneObject.getRotate().y),
                         glm::radians(spotLightSceneObject.getRotate().z)) *
      glm::vec4{spotLightPtr->getDirection(), 0.0f}
  });
  shadowMapCamera.setVerticalFOV(2.0f * spotLightPtr->getAngle());
  shadowMapCamera.setAspectRatio(1.0f);
  shadowMapCamera.setNearPlane(0.01f);
  shadowMapCamera.setFarPlane(2.0f * kShadowMapDistance);

  // Pushing light VP matrix to vector
  spotLightVPMatrices.push_back(shadowMapCamera.getProjectionMatrix() *
                                shadowMapCamera.getViewMatrix());

  std::vector<GLuint> initShaderPrograms{};
  initShaderPrograms.resize(sceneObjects.size());

  // Temporary changing scene objects shader programs and updating shaders camera
  for (size_t i = 0; i < sceneObjects.size(); ++i) {
    if (sceneObjects[i].getMeshPtr() != nullptr) {
      initShaderPrograms[i] = sceneObjects[i].getMeshPtr()->getShaderProgram();
      sceneObjects[i].getMeshPtr()->setShaderProgram(spotLightShadowMapShaderProgram);
    }
  }
  SceneObject::updateShadersCamera(sceneObjects, shadowMapCamera);

  // Rendering scene objects from camera point of view and reverting shader program changes
  for (size_t i = 0; i < sceneObjects.size(); ++i) {
    if (sceneObjects[i].getMeshPtr() != nullptr) {
      sceneObjects[i].render();
      sceneObjects[i].getMeshPtr()->setShaderProgram(initShaderPrograms[i]);
    }
  }

  // Unbinding shadow map framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void renderDirectionalLightShadowMap(const std::vector<SceneObject> &sceneObjects,
                                            std::vector<glm::mat4> &directionalLightVPMatrices,
                                            const SceneObject      &directionalLightSceneObject,
                                            const DirectionalLight *directionalLightPtr,
                                            GLuint           directionalLightShadowMapShaderProgram,
                                            const glm::vec3 &viewPos) {
  // Setting shadow map viewport
  glViewport(0, 0, directionalLightPtr->getShadowMapTextureResolution(),
             directionalLightPtr->getShadowMapTextureResolution());

  // Binding and clearing shadow map framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, directionalLightPtr->getShadowMapFBO());
  glClear(GL_DEPTH_BUFFER_BIT);

  // Configuring camera viewing from light position in light direction
  OrthographicCamera shadowMapCamera{};
  glm::vec3          lightDir{
      glm::eulerAngleXYZ(glm::radians(directionalLightSceneObject.getRotate().x),
                                  glm::radians(directionalLightSceneObject.getRotate().y),
                                  glm::radians(directionalLightSceneObject.getRotate().z)) *
      glm::vec4{directionalLightPtr->getDirection(), 0.0f}
  };
  shadowMapCamera.setPosition(-glm::normalize(lightDir) * kShadowMapDistance + viewPos);
  shadowMapCamera.setWorldUp(kUp);
  shadowMapCamera.look(lightDir);
  shadowMapCamera.setLeftBorder(-kShadowMapDistance);
  shadowMapCamera.setRightBorder(kShadowMapDistance);
  shadowMapCamera.setBottomBorder(-kShadowMapDistance);
  shadowMapCamera.setTopBorder(kShadowMapDistance);
  shadowMapCamera.setNearPlane(0.0f);
  shadowMapCamera.setFarPlane(2.0f * kShadowMapDistance);

  // Pushing light VP matrix to vector
  directionalLightVPMatrices.push_back(shadowMapCamera.getProjectionMatrix() *
                                       shadowMapCamera.getViewMatrix());

  std::vector<GLuint> initShaderPrograms{};
  initShaderPrograms.resize(sceneObjects.size());

  // Temporary changing scene objects shader programs and updating shaders camera
  for (size_t i = 0; i < sceneObjects.size(); ++i) {
    if (sceneObjects[i].getMeshPtr() != nullptr) {
      initShaderPrograms[i] = sceneObjects[i].getMeshPtr()->getShaderProgram();
      sceneObjects[i].getMeshPtr()->setShaderProgram(directionalLightShadowMapShaderProgram);
    }
  }
  SceneObject::updateShadersCamera(sceneObjects, shadowMapCamera);

  // Rendering scene objects from camera point of view and reverting shader program changes
  for (size_t i = 0; i < sceneObjects.size(); ++i) {
    if (sceneObjects[i].getMeshPtr() != nullptr) {
      sceneObjects[i].render();
      sceneObjects[i].getMeshPtr()->setShaderProgram(initShaderPrograms[i]);
    }
  }

  // Unbinding shadow map framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void renderPointLightShadowMap(const std::vector<SceneObject> &sceneObjects,
                                      std::vector<float>             &pointLightFarPlanes,
                                      const SceneObject              &pointLightSceneObject,
                                      const PointLight               *pointLightPtr,
                                      GLuint pointLightShadowMapShaderProgram) {
  // Setting shadow map viewport
  glViewport(0, 0, pointLightPtr->getShadowMapTextureResolution(),
             pointLightPtr->getShadowMapTextureResolution());

  // Binding and clearing shadow map framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, pointLightPtr->getShadowMapFBO());
  glClear(GL_DEPTH_BUFFER_BIT);

  // Configuring camera viewing from light position in light direction
  PerspectiveCamera shadowMapCamera{};
  shadowMapCamera.setPosition(pointLightSceneObject.getTranslate());
  shadowMapCamera.setVerticalFOV(glm::radians(90.0f));
  shadowMapCamera.setAspectRatio(1.0f);
  shadowMapCamera.setNearPlane(0.01f);
  shadowMapCamera.setFarPlane(2.0f * kShadowMapDistance);

  glm::mat4              projMatrix{shadowMapCamera.getProjectionMatrix()};
  std::vector<glm::mat4> pointLightVPMatrices{};
  // For each cube map face
  for (unsigned int i = 0; i < 6; ++i) {
    switch (i) {
      case 0:
        shadowMapCamera.setForward(kRight);
        shadowMapCamera.setUp(kUp);
        break;
      case 1:
        shadowMapCamera.setForward(-kRight);
        shadowMapCamera.setUp(kUp);
        break;
      case 2:
        shadowMapCamera.setForward(-kUp);
        shadowMapCamera.setUp(kForward);
        break;
      case 3:
        shadowMapCamera.setForward(kUp);
        shadowMapCamera.setUp(-kForward);
        break;
      case 4:
        shadowMapCamera.setForward(kForward);
        shadowMapCamera.setUp(kUp);
        break;
      case 5:
        shadowMapCamera.setForward(-kForward);
        shadowMapCamera.setUp(kUp);
        break;
    }

    pointLightVPMatrices.push_back(projMatrix * shadowMapCamera.getViewMatrix());
  }

  // Pushing light camera far plane to vector
  pointLightFarPlanes.push_back(shadowMapCamera.getFarPlane());

  // Updating shader program uniform values
  glUseProgram(pointLightShadowMapShaderProgram);
  for (unsigned int i = 0; i < 6; ++i) {
    glUniformMatrix4fv(glGetUniformLocation(pointLightShadowMapShaderProgram,
                                            ("LIGHT_VP[" + std::to_string(i) + "]").c_str()),
                       1, GL_FALSE, glm::value_ptr(pointLightVPMatrices[i]));
    glUniform3fv(glGetUniformLocation(pointLightShadowMapShaderProgram, "LIGHT.worldPos"), 1,
                 glm::value_ptr(pointLightSceneObject.getTranslate()));
    glUniform1f(glGetUniformLocation(pointLightShadowMapShaderProgram, "LIGHT.farPlane"),
                shadowMapCamera.getFarPlane());
  }
  glUseProgram(0);

  std::vector<GLuint> initShaderPrograms{};
  initShaderPrograms.resize(sceneObjects.size());

  // Temporary changing scene objects shader programs and updating shaders camera
  for (size_t i = 0; i < sceneObjects.size(); ++i) {
    if (sceneObjects[i].getMeshPtr() != nullptr) {
      initShaderPrograms[i] = sceneObjects[i].getMeshPtr()->getShaderProgram();
      sceneObjects[i].getMeshPtr()->setShaderProgram(pointLightShadowMapShaderProgram);
    }
  }
  SceneObject::updateShadersCamera(sceneObjects, shadowMapCamera);

  // Rendering scene objects from camera point of view and reverting shader program changes
  for (size_t i = 0; i < sceneObjects.size(); ++i) {
    if (sceneObjects[i].getMeshPtr() != nullptr) {
      sceneObjects[i].render();
      sceneObjects[i].getMeshPtr()->setShaderProgram(initShaderPrograms[i]);
    }
  }

  // Unbinding shadow map framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void updateShaderProgramSpotLights(GLuint                                  shaderProgram,
                                   const std::vector<const SceneObject *> &spotLightSceneObjectPtrs,
                                   GLuint                  spotLightShadowMapShaderProgram,
                                   int                    &currShadowMapTextureUnit,
                                   std::vector<glm::mat4> &spotLightVPMatrices) {
  for (size_t i = 0; i < spotLightSceneObjectPtrs.size() && i < kMaxSpotLightCount; ++i) {
    const SceneObject &sceneObject = *spotLightSceneObjectPtrs[i];
    const SpotLight   *spotLightPtr =
        dynamic_cast<const SpotLight *>(sceneObject.getLightPtr().get());

    glUniform3fv(glGetUniformLocation(shaderProgram,
                                      ("SPOT_LIGHTS[" + std::to_string(i) + "].worldPos").c_str()),
                 1, glm::value_ptr(sceneObject.getTranslate()));
    glUniform3fv(glGetUniformLocation(shaderProgram,
                                      ("SPOT_LIGHTS[" + std::to_string(i) + "].color").c_str()),
                 1, glm::value_ptr(spotLightPtr->getColor()));
    glUniform1f(glGetUniformLocation(shaderProgram,
                                     ("SPOT_LIGHTS[" + std::to_string(i) + "].intensity").c_str()),
                spotLightPtr->getIntensity());
    glm::mat4 rotateMatrix{glm::eulerAngleXYZ(glm::radians(sceneObject.getRotate().x),
                                              glm::radians(sceneObject.getRotate().y),
                                              glm::radians(sceneObject.getRotate().z))};
    glm::vec3 dir{
        rotateMatrix * glm::vec4{spotLightPtr->getDirection(), 0.0f}
    };
    glUniform3fv(
        glGetUniformLocation(shaderProgram, ("SPOT_LIGHTS[" + std::to_string(i) + "].dir").c_str()),
        1, glm::value_ptr(dir));
    glUniform1f(glGetUniformLocation(shaderProgram,
                                     ("SPOT_LIGHTS[" + std::to_string(i) + "].linAttCoef").c_str()),
                spotLightPtr->getLinAttCoef());
    glUniform1f(glGetUniformLocation(
                    shaderProgram, ("SPOT_LIGHTS[" + std::to_string(i) + "].quadAttCoef").c_str()),
                spotLightPtr->getQuadAttCoef());
    glUniform1f(glGetUniformLocation(shaderProgram,
                                     ("SPOT_LIGHTS[" + std::to_string(i) + "].angle").c_str()),
                spotLightPtr->getAngle());
    glUniform1f(glGetUniformLocation(
                    shaderProgram, ("SPOT_LIGHTS[" + std::to_string(i) + "].smoothAngle").c_str()),
                spotLightPtr->getSmoothAngle());

    // If shader program is specified
    if (spotLightShadowMapShaderProgram > 0) {
      glUniform1i(glGetUniformLocation(
                      shaderProgram, ("SPOT_LIGHTS[" + std::to_string(i) + "].shadowMap").c_str()),
                  currShadowMapTextureUnit);
      glUniformMatrix4fv(glGetUniformLocation(
                             shaderProgram, ("SPOT_LIGHTS[" + std::to_string(i) + "].VP").c_str()),
                         1, GL_FALSE, glm::value_ptr(spotLightVPMatrices.back()));

      // Deleting element from back
      spotLightVPMatrices.pop_back();

      // Binding shadow map texture
      glActiveTexture(GL_TEXTURE0 + currShadowMapTextureUnit);
      glBindTexture(GL_TEXTURE_2D, sceneObject.getLightPtr()->getShadowMapTexture());

      // Incrementing current shadow map texture unit
      ++currShadowMapTextureUnit;
    }
  }
}

void updateShaderProgramDirectionalLights(
    GLuint shaderProgram, const std::vector<const SceneObject *> &directionalLightSceneObjectPtrs,
    GLuint directionalLightShadowMapShaderProgram, int &currShadowMapTextureUnit,
    std::vector<glm::mat4> &directionalLightVPMatrices) {
  for (size_t i = 0; i < directionalLightSceneObjectPtrs.size() && i < kMaxDirectionLightCount;
       ++i) {
    const SceneObject      &sceneObject = *directionalLightSceneObjectPtrs[i];
    const DirectionalLight *directionalLightPtr =
        dynamic_cast<const DirectionalLight *>(sceneObject.getLightPtr().get());

    glUniform3fv(
        glGetUniformLocation(shaderProgram,
                             ("DIRECTIONAL_LIGHTS[" + std::to_string(i) + "].color").c_str()),
        1, glm::value_ptr(directionalLightPtr->getColor()));
    glUniform1f(
        glGetUniformLocation(shaderProgram,
                             ("DIRECTIONAL_LIGHTS[" + std::to_string(i) + "].intensity").c_str()),
        directionalLightPtr->getIntensity());
    glm::mat4 rotateMatrix{glm::eulerAngleXYZ(glm::radians(sceneObject.getRotate().x),
                                              glm::radians(sceneObject.getRotate().y),
                                              glm::radians(sceneObject.getRotate().z))};
    glm::vec3 dir{
        rotateMatrix * glm::vec4{directionalLightPtr->getDirection(), 0.0f}
    };
    glUniform3fv(glGetUniformLocation(
                     shaderProgram, ("DIRECTIONAL_LIGHTS[" + std::to_string(i) + "].dir").c_str()),
                 1, glm::value_ptr(dir));

    // If shader program is specified
    if (directionalLightShadowMapShaderProgram > 0) {
      glUniform1i(
          glGetUniformLocation(shaderProgram,
                               ("DIRECTIONAL_LIGHTS[" + std::to_string(i) + "].shadowMap").c_str()),
          currShadowMapTextureUnit);
      glUniformMatrix4fv(
          glGetUniformLocation(shaderProgram,
                               ("DIRECTIONAL_LIGHTS[" + std::to_string(i) + "].VP").c_str()),
          1, GL_FALSE, glm::value_ptr(directionalLightVPMatrices.back()));

      // Deleting element from back
      directionalLightVPMatrices.pop_back();

      // Binding shadow map texture
      glActiveTexture(GL_TEXTURE0 + currShadowMapTextureUnit);
      glBindTexture(GL_TEXTURE_2D, sceneObject.getLightPtr()->getShadowMapTexture());

      // Incrementing current shadow map texture unit
      ++currShadowMapTextureUnit;
    }
  }
}

void updateShaderProgramPointLights(
    GLuint shaderProgram, const std::vector<const SceneObject *> &pointLightSceneObjectPtrs,
    GLuint pointLightShadowMapShaderProgram, int &currShadowMapTextureUnit,
    std::vector<float> &pointLightFarPlanes) {
  for (size_t i = 0; i < pointLightSceneObjectPtrs.size() && i < kMaxPointLightCount; ++i) {
    const SceneObject &sceneObject = *pointLightSceneObjectPtrs[i];
    const PointLight  *pointLightPtr =
        dynamic_cast<const PointLight *>(sceneObject.getLightPtr().get());

    glUniform3fv(glGetUniformLocation(shaderProgram,
                                      ("POINT_LIGHTS[" + std::to_string(i) + "].worldPos").c_str()),
                 1, glm::value_ptr(sceneObject.getTranslate()));
    glUniform3fv(glGetUniformLocation(shaderProgram,
                                      ("POINT_LIGHTS[" + std::to_string(i) + "].color").c_str()),
                 1, glm::value_ptr(pointLightPtr->getColor()));
    glUniform1f(glGetUniformLocation(shaderProgram,
                                     ("POINT_LIGHTS[" + std::to_string(i) + "].intensity").c_str()),
                pointLightPtr->getIntensity());
    glUniform1f(glGetUniformLocation(
                    shaderProgram, ("POINT_LIGHTS[" + std::to_string(i) + "].linAttCoef").c_str()),
                pointLightPtr->getLinAttCoef());
    glUniform1f(glGetUniformLocation(
                    shaderProgram, ("POINT_LIGHTS[" + std::to_string(i) + "].quadAttCoef").c_str()),
                pointLightPtr->getQuadAttCoef());

    // If shader program is specified
    if (pointLightShadowMapShaderProgram > 0) {
      glUniform1i(glGetUniformLocation(
                      shaderProgram, ("POINT_LIGHTS[" + std::to_string(i) + "].shadowMap").c_str()),
                  currShadowMapTextureUnit);
      glUniform1f(glGetUniformLocation(
                      shaderProgram, ("POINT_LIGHTS[" + std::to_string(i) + "].farPlane").c_str()),
                  pointLightFarPlanes.back());

      // Deleting element from back
      pointLightFarPlanes.pop_back();

      // Binding shadow map texture
      glActiveTexture(GL_TEXTURE0 + currShadowMapTextureUnit);
      glBindTexture(GL_TEXTURE_CUBE_MAP, sceneObject.getLightPtr()->getShadowMapTexture());

      // Incrementing current shadow map texture unit
      ++currShadowMapTextureUnit;
    }
  }
}
