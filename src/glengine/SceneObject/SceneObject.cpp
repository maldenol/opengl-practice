// Header file
#include "./SceneObject.hpp"

// STD
#include <utility>

// GLM
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

using namespace glengine;

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

void SceneObject::updateShadersCamera(const std::vector<SceneObject> &sceneObjects,
                                      const BaseCamera               &camera) noexcept {
  // Getting camera view and projection matrices
  const glm::mat4 viewMatrix{camera.getViewMatrix()};
  const glm::mat4 projMatrix{camera.getProjectionMatrix()};

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
      glUniform3fv(glGetUniformLocation(shaderProgram, "VIEW_POS"), 1,
                   glm::value_ptr(camera.getPos()));

      glUseProgram(0);
    }
  }
}

void SceneObject::updateShadersLights(const std::vector<SceneObject> &sceneObjects) noexcept {
  // Getting shader programs and light sources
  std::vector<GLuint>      shaderPrograms{};
  std::vector<SceneObject> directionalLightSceneObjects{};
  std::vector<SceneObject> pointLightSceneObjects{};
  std::vector<SceneObject> spotLightSceneObjects{};
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

    if (lightPtr == nullptr) continue;

    // If light is spot (must be before directional and point light)
    const SpotLight *spotLightPtr = dynamic_cast<const SpotLight *>(lightPtr);
    if (spotLightPtr != nullptr) {
      spotLightSceneObjects.push_back(sceneObject);

      continue;
    }

    // If light is directional
    const DirectionalLight *direcionalLightPtr = dynamic_cast<const DirectionalLight *>(lightPtr);
    if (direcionalLightPtr != nullptr) {
      directionalLightSceneObjects.push_back(sceneObject);

      continue;
    }

    // If light is point
    const PointLight *pointLightPtr = dynamic_cast<const PointLight *>(lightPtr);
    if (pointLightPtr != nullptr) {
      pointLightSceneObjects.push_back(sceneObject);

      continue;
    }
  }

  // Updating shader programs uniform values
  for (size_t i = 0; i < shaderPrograms.size(); ++i) {
    const GLuint shaderProgram = shaderPrograms[i];

    glUseProgram(shaderProgram);

    // Skybox
    glUniform1i(glGetUniformLocation(shaderProgram, "SKYBOX"), 6);

    // Ambient light
    glUniform3fv(glGetUniformLocation(shaderProgram, "AMBIENT_LIGHT.color"), 1,
                 glm::value_ptr(glm::vec3{1.0f, 1.0f, 1.0f}));
    glUniform1f(glGetUniformLocation(shaderProgram, "AMBIENT_LIGHT.intensity"), 1.0f);

    // Directional lights
    for (size_t i = 0; i < directionalLightSceneObjects.size(); ++i) {
      const SceneObject      &sceneObject = directionalLightSceneObjects[i];
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
      glUniform3fv(
          glGetUniformLocation(shaderProgram,
                               ("DIRECTIONAL_LIGHTS[" + std::to_string(i) + "].dir").c_str()),
          1, glm::value_ptr(dir));
    }

    // Point lights
    for (size_t i = 0; i < pointLightSceneObjects.size(); ++i) {
      const SceneObject &sceneObject = pointLightSceneObjects[i];
      const PointLight  *pointLightPtr =
          dynamic_cast<const PointLight *>(sceneObject.getLightPtr().get());

      glUniform3fv(glGetUniformLocation(
                       shaderProgram, ("POINT_LIGHTS[" + std::to_string(i) + "].worldPos").c_str()),
                   1, glm::value_ptr(sceneObject.getTranslate()));
      glUniform3fv(glGetUniformLocation(shaderProgram,
                                        ("POINT_LIGHTS[" + std::to_string(i) + "].color").c_str()),
                   1, glm::value_ptr(pointLightPtr->getColor()));
      glUniform1f(glGetUniformLocation(
                      shaderProgram, ("POINT_LIGHTS[" + std::to_string(i) + "].intensity").c_str()),
                  pointLightPtr->getIntensity());
      glUniform1f(
          glGetUniformLocation(shaderProgram,
                               ("POINT_LIGHTS[" + std::to_string(i) + "].linAttCoef").c_str()),
          pointLightPtr->getLinAttCoef());
      glUniform1f(
          glGetUniformLocation(shaderProgram,
                               ("POINT_LIGHTS[" + std::to_string(i) + "].quadAttCoef").c_str()),
          pointLightPtr->getQuadAttCoef());
    }

    // Spot lights
    for (size_t i = 0; i < spotLightSceneObjects.size(); ++i) {
      const SceneObject &sceneObject = spotLightSceneObjects[i];
      const SpotLight   *spotLightPtr =
          dynamic_cast<const SpotLight *>(sceneObject.getLightPtr().get());

      glUniform3fv(glGetUniformLocation(
                       shaderProgram, ("SPOT_LIGHTS[" + std::to_string(i) + "].worldPos").c_str()),
                   1, glm::value_ptr(sceneObject.getTranslate()));
      glUniform3fv(glGetUniformLocation(shaderProgram,
                                        ("SPOT_LIGHTS[" + std::to_string(i) + "].color").c_str()),
                   1, glm::value_ptr(spotLightPtr->getColor()));
      glUniform1f(glGetUniformLocation(
                      shaderProgram, ("SPOT_LIGHTS[" + std::to_string(i) + "].intensity").c_str()),
                  spotLightPtr->getIntensity());
      glm::mat4 rotateMatrix{glm::eulerAngleXYZ(glm::radians(sceneObject.getRotate().x),
                                                glm::radians(sceneObject.getRotate().y),
                                                glm::radians(sceneObject.getRotate().z))};
      glm::vec3 dir{
          rotateMatrix * glm::vec4{spotLightPtr->getDirection(), 0.0f}
      };
      glUniform3fv(glGetUniformLocation(shaderProgram,
                                        ("SPOT_LIGHTS[" + std::to_string(i) + "].dir").c_str()),
                   1, glm::value_ptr(dir));
      glUniform1f(glGetUniformLocation(
                      shaderProgram, ("SPOT_LIGHTS[" + std::to_string(i) + "].linAttCoef").c_str()),
                  spotLightPtr->getLinAttCoef());
      glUniform1f(
          glGetUniformLocation(shaderProgram,
                               ("SPOT_LIGHTS[" + std::to_string(i) + "].quadAttCoef").c_str()),
          spotLightPtr->getQuadAttCoef());
      glUniform1f(glGetUniformLocation(shaderProgram,
                                       ("SPOT_LIGHTS[" + std::to_string(i) + "].angle").c_str()),
                  glm::radians(spotLightPtr->getAngle()));
      glUniform1f(
          glGetUniformLocation(shaderProgram,
                               ("SPOT_LIGHTS[" + std::to_string(i) + "].smoothAngle").c_str()),
          glm::radians(spotLightPtr->getSmoothAngle()));
    }

    glUseProgram(0);
  }
}
