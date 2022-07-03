// Header file
#include "./BaseCamera.hpp"

// STD
#include <utility>

// GLM
#include <glm/gtc/matrix_transform.hpp>

glengine::BaseCamera::BaseCamera() noexcept : _pos{glm::vec3{0.0f}}, _worldUp{glm::vec3{0.0f, 1.0f, 0.0f}} {
  this->lookAt(glm::vec3{0.0f, 0.0f, -1.0f});
}

glengine::BaseCamera::BaseCamera(const BaseCamera &camera) noexcept
    : _pos{camera._pos},
      _worldUp{camera._worldUp},
      _forward{camera._forward},
      _right{camera._right},
      _up{camera._up} {}

glengine::BaseCamera &glengine::BaseCamera::operator=(const BaseCamera &camera) noexcept {
  _pos     = camera._pos;
  _worldUp = camera._worldUp;
  _forward = camera._forward;
  _right   = camera._right;
  _up      = camera._up;

  return *this;
}

glengine::BaseCamera::BaseCamera(BaseCamera &&camera) noexcept
    : _pos{std::exchange(camera._pos, glm::vec3{})},
      _worldUp{std::exchange(camera._worldUp, glm::vec3{})},
      _forward{std::exchange(camera._forward, glm::vec3{})},
      _right{std::exchange(camera._right, glm::vec3{})},
      _up{std::exchange(camera._up, glm::vec3{})} {}

glengine::BaseCamera &glengine::BaseCamera::operator=(BaseCamera &&camera) noexcept {
  std::swap(_pos, camera._pos);
  std::swap(_worldUp, camera._worldUp);
  std::swap(_forward, camera._forward);
  std::swap(_right, camera._right);
  std::swap(_up, camera._up);

  return *this;
}

glengine::BaseCamera::~BaseCamera() noexcept {}

void glengine::BaseCamera::setPosition(const glm::vec3 &pos) noexcept { _pos = pos; }

void glengine::BaseCamera::setWorldUp(const glm::vec3 &worldUp) noexcept {
  _worldUp = glm::normalize(worldUp);
}

void glengine::BaseCamera::look(const glm::vec3 &look) noexcept {
  _forward = glm::normalize(look);
  _right   = glm::cross(_forward, _worldUp);
  _up      = glm::cross(_right, _forward);
}

void glengine::BaseCamera::lookAt(const glm::vec3 &lookAt) noexcept {
  _forward = glm::normalize(lookAt - _pos);
  _right   = glm::cross(_forward, _worldUp);
  _up      = glm::cross(_right, _forward);
}

const glm::vec3 &glengine::BaseCamera::getPosition() const noexcept { return _pos; }

const glm::vec3 &glengine::BaseCamera::getWorldUpDirection() const noexcept { return _worldUp; }

const glm::vec3 &glengine::BaseCamera::getForwardDirection() const noexcept { return _forward; }

const glm::vec3 &glengine::BaseCamera::getRightDirection() const noexcept { return _right; }

const glm::vec3 &glengine::BaseCamera::getUpDirection() const noexcept { return _up; }

glm::mat4 glengine::BaseCamera::getViewMatrix() const noexcept {
  return glm::lookAt(_pos, _pos + _forward, _up);
}
