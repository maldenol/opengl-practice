// Header file
#include "./SceneObject.hpp"

// STD
#include <utility>

// GLM
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

using namespace glengine;

SceneObject::SceneObject() noexcept {}

SceneObject::SceneObject(const glm::vec3 &translate, const glm::vec3 &rotate,
                         const glm::vec3 &scale, const std::shared_ptr<BaseLight> &lightPtr,
                         const std::shared_ptr<Mesh> &meshPtr) noexcept
    : _translate{translate},
      _rotate{rotate},
      _scale{scale},
      _lightPtr{lightPtr},
      _meshPtr{meshPtr} {}

SceneObject::SceneObject(const SceneObject &sceneObject) noexcept
    : _translate{sceneObject._translate},
      _rotate{sceneObject._rotate},
      _scale{sceneObject._scale},
      _lightPtr{sceneObject._lightPtr},
      _meshPtr{sceneObject._meshPtr} {}

SceneObject &SceneObject::operator=(const SceneObject &sceneObject) noexcept {
  _translate = sceneObject._translate;
  _rotate    = sceneObject._rotate;
  _scale     = sceneObject._scale;
  _lightPtr  = sceneObject._lightPtr;
  _meshPtr   = sceneObject._meshPtr;

  return *this;
}

SceneObject::SceneObject(SceneObject &&sceneObject) noexcept
    : _translate{std::exchange(sceneObject._translate, glm::vec3{})},
      _rotate{std::exchange(sceneObject._rotate, glm::vec3{})},
      _scale{std::exchange(sceneObject._scale, glm::vec3{})},
      _lightPtr{std::exchange(sceneObject._lightPtr, std::shared_ptr<BaseLight>{})},
      _meshPtr{std::exchange(sceneObject._meshPtr, std::shared_ptr<Mesh>{})} {}

SceneObject &SceneObject::operator=(SceneObject &&sceneObject) noexcept {
  std::swap(_translate, sceneObject._translate);
  std::swap(_rotate, sceneObject._rotate);
  std::swap(_scale, sceneObject._scale);
  std::swap(_lightPtr, sceneObject._lightPtr);
  std::swap(_meshPtr, sceneObject._meshPtr);

  return *this;
}

SceneObject::~SceneObject() noexcept {}

void SceneObject::setTranslate(const glm::vec3 &translate) noexcept { _translate = translate; }

void SceneObject::setRotate(const glm::vec3 &rotate) noexcept { _rotate = rotate; }

void SceneObject::setScale(const glm::vec3 &scale) noexcept { _scale = scale; }

void SceneObject::setLightPtr(const std::shared_ptr<BaseLight> &lightPtr) noexcept {
  _lightPtr = lightPtr;
}

void SceneObject::setMeshPtr(const std::shared_ptr<Mesh> &meshPtr) noexcept { _meshPtr = meshPtr; }

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

void SceneObject::render(const BaseCamera &camera) const {
  if (_meshPtr == nullptr) return;

  const Mesh::Material &material      = _meshPtr->getMaterial();
  const GLuint          shaderProgram = _meshPtr->getShaderProgram();

  // Setting specific shader program to use for render
  glUseProgram(shaderProgram);
  // Binding VAO with associated EBO and VBO
  glBindVertexArray(_meshPtr->getVAO());
  glBindBuffer(GL_ARRAY_BUFFER, _meshPtr->getVBO());

  // For each texture
  for (size_t i = 0; i < material.textures.size(); ++i) {
    // Binding texture to texture unit
    glActiveTexture(GL_TEXTURE0 + material.textures[i].index);
    glBindTexture(GL_TEXTURE_2D, material.textures[i].texture);
  }

  // Calculating mesh model matrix
  glm::mat4x4 modelMatrix{1.0f};
  modelMatrix = glm::translate(modelMatrix, _translate);
  modelMatrix = modelMatrix * glm::eulerAngleXYZ(glm::radians(_rotate.x), glm::radians(_rotate.y),
                                                 glm::radians(_rotate.z));
  modelMatrix = glm::scale(modelMatrix, _scale);

  // Updating shader uniform variables
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE,
                     glm::value_ptr(modelMatrix));
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE,
                     glm::value_ptr(camera.getViewMatrix()));
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "proj"), 1, GL_FALSE,
                     glm::value_ptr(camera.getProjectionMatrix()));

  // Drawing mesh
  glDrawElements(GL_TRIANGLES, _meshPtr->getIndexCount(), GL_UNSIGNED_INT, 0);

  // Unbinding configured VAO and VBO
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // Unbinding shader program
  glUseProgram(0);
}
