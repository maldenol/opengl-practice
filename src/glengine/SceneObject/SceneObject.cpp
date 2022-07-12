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
SceneObject::SceneObject() noexcept { recalculateMVP(); }

// Parameterized constructor
SceneObject::SceneObject(const glm::vec3 &translate, const glm::vec3 &rotate,
                         const glm::vec3 &scale, const std::shared_ptr<BaseLight> &lightPtr,
                         const std::shared_ptr<Mesh> &meshPtr) noexcept
    : _translate{translate},
      _rotate{rotate},
      _scale{scale},
      _lightPtr{lightPtr},
      _meshPtr{meshPtr} {
  recalculateMVP();
}

// Copy constructor
SceneObject::SceneObject(const SceneObject &sceneObject) noexcept
    : _translate{sceneObject._translate},
      _rotate{sceneObject._rotate},
      _scale{sceneObject._scale},
      _MVP{sceneObject._MVP},
      _lightPtr{sceneObject._lightPtr},
      _meshPtr{sceneObject._meshPtr} {}

// Copy assignment operator
SceneObject &SceneObject::operator=(const SceneObject &sceneObject) noexcept {
  _translate = sceneObject._translate;
  _rotate    = sceneObject._rotate;
  _scale     = sceneObject._scale;
  _MVP       = sceneObject._MVP;
  _lightPtr  = sceneObject._lightPtr;
  _meshPtr   = sceneObject._meshPtr;

  return *this;
}

// Move constructor
SceneObject::SceneObject(SceneObject &&sceneObject) noexcept
    : _translate{std::exchange(sceneObject._translate, glm::vec3{})},
      _rotate{std::exchange(sceneObject._rotate, glm::vec3{})},
      _scale{std::exchange(sceneObject._scale, glm::vec3{})},
      _MVP{std::exchange(sceneObject._MVP, glm::mat4{})},
      _lightPtr{std::exchange(sceneObject._lightPtr, std::shared_ptr<BaseLight>{})},
      _meshPtr{std::exchange(sceneObject._meshPtr, std::shared_ptr<Mesh>{})} {}

// Move assignment operator
SceneObject &SceneObject::operator=(SceneObject &&sceneObject) noexcept {
  std::swap(_translate, sceneObject._translate);
  std::swap(_rotate, sceneObject._rotate);
  std::swap(_scale, sceneObject._scale);
  std::swap(_MVP, sceneObject._MVP);
  std::swap(_lightPtr, sceneObject._lightPtr);
  std::swap(_meshPtr, sceneObject._meshPtr);

  return *this;
}

// Destructor
SceneObject::~SceneObject() noexcept {}

// Setters

void SceneObject::setTranslate(const glm::vec3 &translate) noexcept {
  _translate = translate;
  recalculateMVP();
}

void SceneObject::setRotate(const glm::vec3 &rotate) noexcept {
  _rotate = rotate;
  recalculateMVP();
}

void SceneObject::setScale(const glm::vec3 &scale) noexcept {
  _scale = scale;
  recalculateMVP();
}

void SceneObject::setMVP(const glm::mat4 &matrixMVP) noexcept { _MVP = matrixMVP; }

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

const glm::mat4 &SceneObject::getMVP() const noexcept { return _MVP; }

glm::mat4 &SceneObject::getMVP() noexcept { return _MVP; }

const std::shared_ptr<BaseLight> &SceneObject::getLightPtr() const noexcept { return _lightPtr; }

std::shared_ptr<BaseLight> &SceneObject::getLightPtr() noexcept { return _lightPtr; }

const std::shared_ptr<Mesh> &SceneObject::getMeshPtr() const noexcept { return _meshPtr; }

std::shared_ptr<Mesh> &SceneObject::getMeshPtr() noexcept { return _meshPtr; }

// Other member functions

void SceneObject::recalculateMVP() noexcept {
  // Calculating scene object model matrix
  _MVP = glm::mat4{1.0f};
  _MVP = glm::translate(_MVP, _translate);
  _MVP = _MVP * glm::eulerAngleXYZ(glm::radians(_rotate.x), glm::radians(_rotate.y),
                                   glm::radians(_rotate.z));
  _MVP = glm::scale(_MVP, _scale);
}

void SceneObject::updateShaderMVP(const BaseCamera &camera) const noexcept {
  // Updating object shader program uniform values
  const GLuint shaderProgram = _meshPtr->getShaderProgram();
  glUseProgram(shaderProgram);

  glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MODEL"), 1, GL_FALSE,
                     glm::value_ptr(_MVP));
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "VIEW"), 1, GL_FALSE,
                     glm::value_ptr(camera.getViewMatrix()));
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "PROJ"), 1, GL_FALSE,
                     glm::value_ptr(camera.getProjectionMatrix()));

  glUseProgram(0);
}

void SceneObject::updateShaderViewPos(const BaseCamera &camera) const noexcept {
  // Updating object shader program uniform values
  const GLuint shaderProgram = _meshPtr->getShaderProgram();
  glUseProgram(shaderProgram);

  glUniform3fv(glGetUniformLocation(shaderProgram, "VIEW_POS"), 1, glm::value_ptr(camera.getPos()));

  glUseProgram(0);
}

void SceneObject::updateShaderLights(const std::vector<SceneObject> &sceneObjects) const noexcept {
  // Getting light sources
  std::vector<SceneObject> directionalLightSceneObjects{};
  std::vector<SceneObject> pointLightSceneObjects{};
  std::vector<SceneObject> spotLightSceneObjects{};
  for (size_t i = 0; i < sceneObjects.size(); ++i) {
    if (sceneObjects[i].getLightPtr() == nullptr) continue;

    const SceneObject &sceneObject = sceneObjects[i];
    const BaseLight   *lightPtr    = sceneObject.getLightPtr().get();

    // If light is spot (must be before directional and point light)
    const SpotLight *spotLight = dynamic_cast<const SpotLight *>(lightPtr);
    if (spotLight != nullptr) {
      spotLightSceneObjects.push_back(sceneObject);

      continue;
    }

    // If light is directional
    const DirectionalLight *direcionalLight = dynamic_cast<const DirectionalLight *>(lightPtr);
    if (direcionalLight != nullptr) {
      directionalLightSceneObjects.push_back(sceneObject);

      continue;
    }

    // If light is point
    const PointLight *pointLight = dynamic_cast<const PointLight *>(lightPtr);
    if (pointLight != nullptr) {
      pointLightSceneObjects.push_back(sceneObject);

      continue;
    }
  }

  // Updating object shader program uniform values
  const GLuint shaderProgram = _meshPtr->getShaderProgram();
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
    const DirectionalLight *directionalLight =
        dynamic_cast<const DirectionalLight *>(sceneObject.getLightPtr().get());

    glUniform3fv(
        glGetUniformLocation(shaderProgram,
                             ("DIRECTIONAL_LIGHTS[" + std::to_string(i) + "].color").c_str()),
        1, glm::value_ptr(directionalLight->getColor()));
    glUniform1f(
        glGetUniformLocation(shaderProgram,
                             ("DIRECTIONAL_LIGHTS[" + std::to_string(i) + "].intensity").c_str()),
        directionalLight->getIntensity());
    glm::mat4 rotateMatrix{glm::eulerAngleXYZ(glm::radians(sceneObject.getRotate().x),
                                              glm::radians(sceneObject.getRotate().y),
                                              glm::radians(sceneObject.getRotate().z))};
    glm::vec3 dir{
        rotateMatrix * glm::vec4{directionalLight->getDirection(), 0.0f}
    };
    glUniform3fv(glGetUniformLocation(
                     shaderProgram, ("DIRECTIONAL_LIGHTS[" + std::to_string(i) + "].dir").c_str()),
                 1, glm::value_ptr(dir));
  }

  // Point lights
  for (size_t i = 0; i < pointLightSceneObjects.size(); ++i) {
    const SceneObject &sceneObject = pointLightSceneObjects[i];
    const PointLight  *pointLight =
        dynamic_cast<const PointLight *>(sceneObject.getLightPtr().get());

    glUniform3fv(glGetUniformLocation(shaderProgram,
                                      ("POINT_LIGHTS[" + std::to_string(i) + "].worldPos").c_str()),
                 1, glm::value_ptr(sceneObject.getTranslate()));
    glUniform3fv(glGetUniformLocation(shaderProgram,
                                      ("POINT_LIGHTS[" + std::to_string(i) + "].color").c_str()),
                 1, glm::value_ptr(pointLight->getColor()));
    glUniform1f(glGetUniformLocation(shaderProgram,
                                     ("POINT_LIGHTS[" + std::to_string(i) + "].intensity").c_str()),
                pointLight->getIntensity());
    glUniform1f(glGetUniformLocation(
                    shaderProgram, ("POINT_LIGHTS[" + std::to_string(i) + "].linAttCoef").c_str()),
                pointLight->getLinAttCoef());
    glUniform1f(glGetUniformLocation(
                    shaderProgram, ("POINT_LIGHTS[" + std::to_string(i) + "].quadAttCoef").c_str()),
                pointLight->getQuadAttCoef());
  }

  // Spot lights
  for (size_t i = 0; i < spotLightSceneObjects.size(); ++i) {
    const SceneObject &sceneObject = spotLightSceneObjects[i];
    const SpotLight   *spotLight = dynamic_cast<const SpotLight *>(sceneObject.getLightPtr().get());

    glUniform3fv(glGetUniformLocation(shaderProgram,
                                      ("SPOT_LIGHTS[" + std::to_string(i) + "].worldPos").c_str()),
                 1, glm::value_ptr(sceneObject.getTranslate()));
    glUniform3fv(glGetUniformLocation(shaderProgram,
                                      ("SPOT_LIGHTS[" + std::to_string(i) + "].color").c_str()),
                 1, glm::value_ptr(spotLight->getColor()));
    glUniform1f(glGetUniformLocation(shaderProgram,
                                     ("SPOT_LIGHTS[" + std::to_string(i) + "].intensity").c_str()),
                spotLight->getIntensity());
    glm::mat4 rotateMatrix{glm::eulerAngleXYZ(glm::radians(sceneObject.getRotate().x),
                                              glm::radians(sceneObject.getRotate().y),
                                              glm::radians(sceneObject.getRotate().z))};
    glm::vec3 dir{
        rotateMatrix * glm::vec4{spotLight->getDirection(), 0.0f}
    };
    glUniform3fv(
        glGetUniformLocation(shaderProgram, ("SPOT_LIGHTS[" + std::to_string(i) + "].dir").c_str()),
        1, glm::value_ptr(dir));
    glUniform1f(glGetUniformLocation(shaderProgram,
                                     ("SPOT_LIGHTS[" + std::to_string(i) + "].linAttCoef").c_str()),
                spotLight->getLinAttCoef());
    glUniform1f(glGetUniformLocation(
                    shaderProgram, ("SPOT_LIGHTS[" + std::to_string(i) + "].quadAttCoef").c_str()),
                spotLight->getQuadAttCoef());
    glUniform1f(glGetUniformLocation(shaderProgram,
                                     ("SPOT_LIGHTS[" + std::to_string(i) + "].angle").c_str()),
                glm::radians(spotLight->getAngle()));
    glUniform1f(glGetUniformLocation(
                    shaderProgram, ("SPOT_LIGHTS[" + std::to_string(i) + "].smoothAngle").c_str()),
                glm::radians(spotLight->getSmoothAngle()));
  }

  glUseProgram(0);
}

void SceneObject::render(const BaseCamera &camera, const std::vector<SceneObject> &sceneObjects,
                         unsigned int instanceCount) const noexcept {
  if (_meshPtr == nullptr || !_meshPtr->isComplete()) return;

  // Updating shader uniform variables
  updateShaderMVP(camera);
  updateShaderViewPos(camera);
  updateShaderLights(sceneObjects);

  // Rendering mesh
  _meshPtr->render(instanceCount);
}
