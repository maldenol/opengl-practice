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
SceneObject::SceneObject() noexcept {}

// Parameterized constructor
SceneObject::SceneObject(const glm::vec3 &translate, const glm::vec3 &rotate,
                         const glm::vec3 &scale, const std::shared_ptr<BaseLight> &lightPtr,
                         const std::shared_ptr<Mesh> &meshPtr) noexcept
    : _translate{translate},
      _rotate{rotate},
      _scale{scale},
      _lightPtr{lightPtr},
      _meshPtr{meshPtr} {}

// Copy constructor
SceneObject::SceneObject(const SceneObject &sceneObject) noexcept
    : _translate{sceneObject._translate},
      _rotate{sceneObject._rotate},
      _scale{sceneObject._scale},
      _lightPtr{sceneObject._lightPtr},
      _meshPtr{sceneObject._meshPtr} {}

// Copy assignment operator
SceneObject &SceneObject::operator=(const SceneObject &sceneObject) noexcept {
  _translate = sceneObject._translate;
  _rotate    = sceneObject._rotate;
  _scale     = sceneObject._scale;
  _lightPtr  = sceneObject._lightPtr;
  _meshPtr   = sceneObject._meshPtr;

  return *this;
}

// Move constructor
SceneObject::SceneObject(SceneObject &&sceneObject) noexcept
    : _translate{std::exchange(sceneObject._translate, glm::vec3{})},
      _rotate{std::exchange(sceneObject._rotate, glm::vec3{})},
      _scale{std::exchange(sceneObject._scale, glm::vec3{})},
      _lightPtr{std::exchange(sceneObject._lightPtr, std::shared_ptr<BaseLight>{})},
      _meshPtr{std::exchange(sceneObject._meshPtr, std::shared_ptr<Mesh>{})} {}

// Move assignment operator
SceneObject &SceneObject::operator=(SceneObject &&sceneObject) noexcept {
  std::swap(_translate, sceneObject._translate);
  std::swap(_rotate, sceneObject._rotate);
  std::swap(_scale, sceneObject._scale);
  std::swap(_lightPtr, sceneObject._lightPtr);
  std::swap(_meshPtr, sceneObject._meshPtr);

  return *this;
}

// Destructor
SceneObject::~SceneObject() noexcept {}

// Setters

void SceneObject::setTranslate(const glm::vec3 &translate) noexcept { _translate = translate; }

void SceneObject::setRotate(const glm::vec3 &rotate) noexcept { _rotate = rotate; }

void SceneObject::setScale(const glm::vec3 &scale) noexcept { _scale = scale; }

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

const std::shared_ptr<BaseLight> &SceneObject::getLightPtr() const noexcept { return _lightPtr; }

std::shared_ptr<BaseLight> &SceneObject::getLightPtr() noexcept { return _lightPtr; }

const std::shared_ptr<Mesh> &SceneObject::getMeshPtr() const noexcept { return _meshPtr; }

std::shared_ptr<Mesh> &SceneObject::getMeshPtr() noexcept { return _meshPtr; }

// Other member functions

void SceneObject::render(const BaseCamera               &camera,
                         const std::vector<SceneObject> &sceneObjects) const {
  if (_meshPtr == nullptr) return;

  const Mesh::Material &material      = _meshPtr->getMaterial();
  const GLuint          shaderProgram = _meshPtr->getShaderProgram();

  // Binding VAO with associated EBO and VBO
  glBindVertexArray(_meshPtr->getVAO());
  glBindBuffer(GL_ARRAY_BUFFER, _meshPtr->getVBO());

  // For each texture
  for (size_t i = 0; i < material.textures.size(); ++i) {
    // Binding texture to texture unit
    glActiveTexture(GL_TEXTURE0 + material.textures[i].index);
    glBindTexture(GL_TEXTURE_2D, material.textures[i].texture);
  }

  // Updating shader uniform variables
  updateShaderMVP(camera);
  updateShaderViewPos(camera);
  updateShaderLights(sceneObjects);
  updateShaderMaterial();

  // Setting specific shader program to use for render
  glUseProgram(shaderProgram);

  // Drawing mesh
  glDrawElements(GL_TRIANGLES, _meshPtr->getIndexCount(), GL_UNSIGNED_INT, 0);

  // Unbinding shader program
  glUseProgram(0);

  // Unbinding configured VAO and VBO
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SceneObject::updateShaderMVP(const BaseCamera &camera) const noexcept {
  // Calculating mesh model matrix
  glm::mat4x4 modelMatrix{1.0f};
  modelMatrix = glm::translate(modelMatrix, _translate);
  modelMatrix = modelMatrix * glm::eulerAngleXYZ(glm::radians(_rotate.x), glm::radians(_rotate.y),
                                                 glm::radians(_rotate.z));
  modelMatrix = glm::scale(modelMatrix, _scale);

  // Updating object shader program uniform values
  const GLuint shaderProgram = _meshPtr->getShaderProgram();
  glUseProgram(shaderProgram);

  glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MODEL"), 1, GL_FALSE,
                     glm::value_ptr(modelMatrix));
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

  glUniform3fv(glGetUniformLocation(shaderProgram, "VIEW_POS"), 1,
               glm::value_ptr(camera.getPosition()));

  glUseProgram(0);
}

void SceneObject::updateShaderLights(const std::vector<SceneObject> &sceneObjects) const noexcept {
  // Getting light sources
  std::vector<SceneObject> directionalLightSceneObjects{};
  std::vector<SceneObject> pointLightSceneObjects{};
  std::vector<SceneObject> spotLightSceneObjects{};
  for (unsigned int i = 0; i < sceneObjects.size(); ++i) {
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
  // Directional lights
  for (unsigned int i = 0; i < directionalLightSceneObjects.size(); ++i) {
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
    glm::mat4x4 rotateMatrix{glm::eulerAngleXYZ(glm::radians(sceneObject.getRotate().x),
                                                glm::radians(sceneObject.getRotate().y),
                                                glm::radians(sceneObject.getRotate().z))};
    glm::vec3   dir{
        rotateMatrix * glm::vec4{directionalLight->getDirection(), 0.0f}
    };
    glUniform3fv(glGetUniformLocation(
                     shaderProgram, ("DIRECTIONAL_LIGHTS[" + std::to_string(i) + "].dir").c_str()),
                 1, glm::value_ptr(dir));
  }
  // Point lights
  for (unsigned int i = 0; i < pointLightSceneObjects.size(); ++i) {
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
  for (unsigned int i = 0; i < spotLightSceneObjects.size(); ++i) {
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
    glm::mat4x4 rotateMatrix{glm::eulerAngleXYZ(glm::radians(sceneObject.getRotate().x),
                                                glm::radians(sceneObject.getRotate().y),
                                                glm::radians(sceneObject.getRotate().z))};
    glm::vec3   dir{
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

void SceneObject::updateShaderMaterial() const noexcept {
  // Updating object shader program uniform values
  const GLuint shaderProgram = _meshPtr->getShaderProgram();
  glUseProgram(shaderProgram);

  glUniform1f(glGetUniformLocation(shaderProgram, "MATERIAL.ambCoef"),
              _meshPtr->getMaterial().ambCoef);
  glUniform1f(glGetUniformLocation(shaderProgram, "MATERIAL.diffCoef"),
              _meshPtr->getMaterial().diffCoef);
  glUniform1f(glGetUniformLocation(shaderProgram, "MATERIAL.specCoef"),
              _meshPtr->getMaterial().specCoef);
  glUniform1f(glGetUniformLocation(shaderProgram, "MATERIAL.glossiness"),
              _meshPtr->getMaterial().glossiness);
  glUniform1f(glGetUniformLocation(shaderProgram, "MATERIAL.maxHeight"),
              _meshPtr->getMaterial().maxHeight);
  glUniform1i(glGetUniformLocation(shaderProgram, "MATERIAL.albedoMap"), 0);
  glUniform1i(glGetUniformLocation(shaderProgram, "MATERIAL.normalMap"), 1);
  glUniform1i(glGetUniformLocation(shaderProgram, "MATERIAL.heightMap"), 2);
  glUniform1i(glGetUniformLocation(shaderProgram, "MATERIAL.ambOccMap"), 3);
  glUniform1i(glGetUniformLocation(shaderProgram, "MATERIAL.roughMap"), 4);
  glUniform1i(glGetUniformLocation(shaderProgram, "MATERIAL.emissMap"), 5);

  glUseProgram(0);
}
