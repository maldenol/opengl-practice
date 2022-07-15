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

static constexpr float kDirectionalShadowMapDistance = 20.0f;

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
                                      GLuint shadowMapSP, const glm::vec3 &viewPos) noexcept {
  // Getting shader programs, light sources and light cameras
  std::vector<GLuint>              shaderPrograms{};
  std::vector<const SceneObject *> directionalLightSceneObjectPtrs{};
  std::vector<const SceneObject *> pointLightSceneObjectPtrs{};
  std::vector<const SceneObject *> spotLightSceneObjectPtrs{};
  std::vector<glm::mat4>           directionalLightVPMatrices{};
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

    // If light is spot (must be before directional and point lights)
    const SpotLight *spotLightPtr = dynamic_cast<const SpotLight *>(lightPtr);
    if (spotLightPtr != nullptr) {
      // Adding scene object to spot light vector
      spotLightSceneObjectPtrs.push_back(&sceneObject);

      continue;
    }

    // If light is directional
    const DirectionalLight *direcionalLightPtr = dynamic_cast<const DirectionalLight *>(lightPtr);
    if (direcionalLightPtr != nullptr) {
      // Adding scene object to directional light vector
      directionalLightSceneObjectPtrs.push_back(&sceneObject);

      // Rendering shadow map
      if (shadowMapSP > 0) {
        // Enabling Z- and disabling stencil testing
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDisable(GL_STENCIL_TEST);

        // Setting shadow map viewport
        int viewport[4] = {0};
        glGetIntegerv(GL_VIEWPORT, &viewport[0]);
        glViewport(0, 0, direcionalLightPtr->getShadowMapTextureResolution(),
                   direcionalLightPtr->getShadowMapTextureResolution());

        // Binding and clearing shadow map framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, sceneObject.getLightPtr()->getShadowMapFBO());
        glClear(GL_DEPTH_BUFFER_BIT);

        // Configuring camera viewing from light position in light direction
        OrthographicCamera shadowMapCamera{};
        glm::vec3          lightDir{};
        lightDir =
            dynamic_cast<DirectionalLight *>(sceneObject.getLightPtr().get())->getDirection();
        lightDir = glm::vec3{
            glm::eulerAngleXYZ(glm::radians(sceneObject.getRotate().x),
                               glm::radians(sceneObject.getRotate().y),
                               glm::radians(sceneObject.getRotate().z)) *
            glm::vec4{lightDir, 0.0f}
        };
        lightDir = glm::normalize(lightDir);
        shadowMapCamera.setPosition(-lightDir * kDirectionalShadowMapDistance + viewPos);
        shadowMapCamera.setWorldUp(kUp);
        shadowMapCamera.look(lightDir);
        shadowMapCamera.setLeftBorder(-kDirectionalShadowMapDistance);
        shadowMapCamera.setRightBorder(kDirectionalShadowMapDistance);
        shadowMapCamera.setBottomBorder(-kDirectionalShadowMapDistance);
        shadowMapCamera.setTopBorder(kDirectionalShadowMapDistance);
        shadowMapCamera.setNearPlane(0.0f);
        shadowMapCamera.setFarPlane(2.0f * kDirectionalShadowMapDistance);
        directionalLightVPMatrices.push_back(shadowMapCamera.getProjectionMatrix() *
                                             shadowMapCamera.getViewMatrix());

        std::vector<GLuint> initShaderPrograms{};
        initShaderPrograms.resize(sceneObjects.size());

        // Temporary changing scene objects shader programs and updating shaders camera
        for (size_t j = 0; j < sceneObjects.size(); ++j) {
          if (sceneObjects[j].getMeshPtr() != nullptr) {
            initShaderPrograms[j] = sceneObjects[j].getMeshPtr()->getShaderProgram();
            sceneObjects[j].getMeshPtr()->setShaderProgram(shadowMapSP);
          }
        }
        SceneObject::updateShadersCamera(sceneObjects, shadowMapCamera);

        // Rendering scene objects from camera point of view and revering shader program changes
        for (size_t j = 0; j < sceneObjects.size(); ++j) {
          if (sceneObjects[j].getMeshPtr() != nullptr) {
            sceneObjects[j].render();
            sceneObjects[j].getMeshPtr()->setShaderProgram(initShaderPrograms[j]);
          }
        }

        // Unbinding shadow map framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Reverting viewport changes
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

        // Disabling Z-testing
        glDisable(GL_DEPTH_TEST);
      }

      continue;
    }

    // If light is point
    const PointLight *pointLightPtr = dynamic_cast<const PointLight *>(lightPtr);
    if (pointLightPtr != nullptr) {
      // Adding scene object to point light vector
      pointLightSceneObjectPtrs.push_back(&sceneObject);

      continue;
    }
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

    // Spot lights
    for (size_t j = 0; j < spotLightSceneObjectPtrs.size() && j < kMaxSpotLightCount; ++j) {
      const SceneObject &sceneObject = *spotLightSceneObjectPtrs[j];
      const SpotLight   *spotLightPtr =
          dynamic_cast<const SpotLight *>(sceneObject.getLightPtr().get());

      glUniform3fv(glGetUniformLocation(
                       shaderProgram, ("SPOT_LIGHTS[" + std::to_string(j) + "].worldPos").c_str()),
                   1, glm::value_ptr(sceneObject.getTranslate()));
      glUniform3fv(glGetUniformLocation(shaderProgram,
                                        ("SPOT_LIGHTS[" + std::to_string(j) + "].color").c_str()),
                   1, glm::value_ptr(spotLightPtr->getColor()));
      glUniform1f(glGetUniformLocation(
                      shaderProgram, ("SPOT_LIGHTS[" + std::to_string(j) + "].intensity").c_str()),
                  spotLightPtr->getIntensity());
      glm::mat4 rotateMatrix{glm::eulerAngleXYZ(glm::radians(sceneObject.getRotate().x),
                                                glm::radians(sceneObject.getRotate().y),
                                                glm::radians(sceneObject.getRotate().z))};
      glm::vec3 dir{
          rotateMatrix * glm::vec4{spotLightPtr->getDirection(), 0.0f}
      };
      glUniform3fv(glGetUniformLocation(shaderProgram,
                                        ("SPOT_LIGHTS[" + std::to_string(j) + "].dir").c_str()),
                   1, glm::value_ptr(dir));
      glUniform1f(glGetUniformLocation(
                      shaderProgram, ("SPOT_LIGHTS[" + std::to_string(j) + "].linAttCoef").c_str()),
                  spotLightPtr->getLinAttCoef());
      glUniform1f(
          glGetUniformLocation(shaderProgram,
                               ("SPOT_LIGHTS[" + std::to_string(j) + "].quadAttCoef").c_str()),
          spotLightPtr->getQuadAttCoef());
      glUniform1f(glGetUniformLocation(shaderProgram,
                                       ("SPOT_LIGHTS[" + std::to_string(j) + "].angle").c_str()),
                  glm::radians(spotLightPtr->getAngle()));
      glUniform1f(
          glGetUniformLocation(shaderProgram,
                               ("SPOT_LIGHTS[" + std::to_string(j) + "].smoothAngle").c_str()),
          glm::radians(spotLightPtr->getSmoothAngle()));
    }

    // Directional lights
    for (size_t j = 0; j < directionalLightSceneObjectPtrs.size() && j < kMaxDirectionLightCount;
         ++j) {
      const SceneObject      &sceneObject = *directionalLightSceneObjectPtrs[j];
      const DirectionalLight *directionalLightPtr =
          dynamic_cast<const DirectionalLight *>(sceneObject.getLightPtr().get());

      glUniform3fv(
          glGetUniformLocation(shaderProgram,
                               ("DIRECTIONAL_LIGHTS[" + std::to_string(j) + "].color").c_str()),
          1, glm::value_ptr(directionalLightPtr->getColor()));
      glUniform1f(
          glGetUniformLocation(shaderProgram,
                               ("DIRECTIONAL_LIGHTS[" + std::to_string(j) + "].intensity").c_str()),
          directionalLightPtr->getIntensity());
      glm::mat4 rotateMatrix{glm::eulerAngleXYZ(glm::radians(sceneObject.getRotate().x),
                                                glm::radians(sceneObject.getRotate().y),
                                                glm::radians(sceneObject.getRotate().z))};
      glm::vec3 dir{
          rotateMatrix * glm::vec4{directionalLightPtr->getDirection(), 0.0f}
      };
      glUniform3fv(
          glGetUniformLocation(shaderProgram,
                               ("DIRECTIONAL_LIGHTS[" + std::to_string(j) + "].dir").c_str()),
          1, glm::value_ptr(dir));
      glUniform1i(
          glGetUniformLocation(shaderProgram,
                               ("DIRECTIONAL_LIGHTS[" + std::to_string(0) + "].shadowMap").c_str()),
          currShadowMapTextureUnit);
      glUniformMatrix4fv(
          glGetUniformLocation(shaderProgram,
                               ("DIRECTIONAL_LIGHTS[" + std::to_string(0) + "].VP").c_str()),
          1, GL_FALSE, glm::value_ptr(directionalLightVPMatrices[j]));

      // Binding shadow map texture
      glActiveTexture(GL_TEXTURE0 + currShadowMapTextureUnit);
      glBindTexture(GL_TEXTURE_2D, sceneObject.getLightPtr()->getShadowMapTexture());

      // Incrementing current shadow map texture unit
      ++currShadowMapTextureUnit;
    }

    // Point lights
    for (size_t j = 0; j < pointLightSceneObjectPtrs.size() && j < kMaxPointLightCount; ++j) {
      const SceneObject &sceneObject = *pointLightSceneObjectPtrs[j];
      const PointLight  *pointLightPtr =
          dynamic_cast<const PointLight *>(sceneObject.getLightPtr().get());

      glUniform3fv(glGetUniformLocation(
                       shaderProgram, ("POINT_LIGHTS[" + std::to_string(j) + "].worldPos").c_str()),
                   1, glm::value_ptr(sceneObject.getTranslate()));
      glUniform3fv(glGetUniformLocation(shaderProgram,
                                        ("POINT_LIGHTS[" + std::to_string(j) + "].color").c_str()),
                   1, glm::value_ptr(pointLightPtr->getColor()));
      glUniform1f(glGetUniformLocation(
                      shaderProgram, ("POINT_LIGHTS[" + std::to_string(j) + "].intensity").c_str()),
                  pointLightPtr->getIntensity());
      glUniform1f(
          glGetUniformLocation(shaderProgram,
                               ("POINT_LIGHTS[" + std::to_string(j) + "].linAttCoef").c_str()),
          pointLightPtr->getLinAttCoef());
      glUniform1f(
          glGetUniformLocation(shaderProgram,
                               ("POINT_LIGHTS[" + std::to_string(j) + "].quadAttCoef").c_str()),
          pointLightPtr->getQuadAttCoef());
    }

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
