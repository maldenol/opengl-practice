// Header file
#include <glservice/camera/BaseCamera.hpp>

// STD
#include <utility>

glservice::BaseCamera::BaseCamera() noexcept : _pos{glm::vec3{0.0f}} {
  this->lookAt(glm::vec3{0.0f, 0.0f, -1.0f});
}

glservice::BaseCamera::BaseCamera(const BaseCamera &camera) noexcept
    : _pos{camera._pos},
      _forward{camera._forward},
      _right{camera._right},
      _up{camera._up},
      _lookAt{camera._lookAt} {}

glservice::BaseCamera &glservice::BaseCamera::operator=(
    const BaseCamera &camera) noexcept {
  _pos     = camera._pos;
  _forward = camera._forward;
  _right   = camera._right;
  _up      = camera._up;
  _lookAt  = camera._lookAt;

  return *this;
}

glservice::BaseCamera::BaseCamera(BaseCamera &&camera) noexcept
    : _pos{std::exchange(camera._pos, glm::vec3{})},
      _forward{std::exchange(camera._forward, glm::vec3{})},
      _right{std::exchange(camera._right, glm::vec3{})},
      _up{std::exchange(camera._up, glm::vec3{})},
      _lookAt{std::exchange(camera._lookAt, glm::vec3{})} {}

glservice::BaseCamera &glservice::BaseCamera::operator=(
    BaseCamera &&camera) noexcept {
  std::swap(_pos, camera._pos);
  std::swap(_forward, camera._forward);
  std::swap(_right, camera._right);
  std::swap(_up, camera._up);
  std::swap(_lookAt, camera._lookAt);

  return *this;
}

glservice::BaseCamera::~BaseCamera() noexcept {}

void glservice::BaseCamera::setPosition(const glm::vec3 &pos) noexcept {
  _pos = pos;
}

void glservice::BaseCamera::lookAt(const glm::vec3 &lookAt) noexcept {
  _lookAt  = lookAt;
  _forward = glm::normalize(_lookAt - _pos);
  _right   = glm::cross(_forward, glm::vec3{0.0f, 1.0f, 0.0f});
  _up      = glm::cross(_right, _forward);
}

const glm::vec3 &glservice::BaseCamera::getPosition() const noexcept {
  return _pos;
}

const glm::vec3 &glservice::BaseCamera::getForwardDirection() const noexcept {
  return _forward;
}

const glm::vec3 &glservice::BaseCamera::getRightDirection() const noexcept {
  return _right;
}

const glm::vec3 &glservice::BaseCamera::getUpDirection() const noexcept {
  return _up;
}

const glm::vec3 &glservice::BaseCamera::getLookAtCenter() const noexcept {
  return _lookAt;
}

glm::mat4 glservice::BaseCamera::getViewMatrix() const noexcept {
  return glm::lookAt(_pos, _lookAt, _up);
}
