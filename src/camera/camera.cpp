// Header file
#include "camera.hpp"

// STD
#include <utility>

// BaseCamera

BaseCamera::BaseCamera() noexcept : _pos{glm::vec3{0.0f}} {
  this->lookAt(0.0f, 0.0f, -1.0f);
}

BaseCamera::BaseCamera(const BaseCamera &camera) noexcept
    : _pos{camera._pos},
      _forward{camera._forward},
      _right{camera._right},
      _up{camera._up},
      _lookAt{camera._lookAt} {}

BaseCamera &BaseCamera::operator=(const BaseCamera &camera) noexcept {
  _pos     = camera._pos;
  _forward = camera._forward;
  _right   = camera._right;
  _up      = camera._up;
  _lookAt  = camera._lookAt;

  return *this;
}

BaseCamera::BaseCamera(BaseCamera &&camera) noexcept
    : _pos{std::exchange(camera._pos, glm::vec3{})},
      _forward{std::exchange(camera._forward, glm::vec3{})},
      _right{std::exchange(camera._right, glm::vec3{})},
      _up{std::exchange(camera._up, glm::vec3{})},
      _lookAt{std::exchange(camera._lookAt, glm::vec3{})} {}

BaseCamera &BaseCamera::operator=(BaseCamera &&camera) noexcept {
  std::swap(_pos, camera._pos);
  std::swap(_forward, camera._forward);
  std::swap(_right, camera._right);
  std::swap(_up, camera._up);
  std::swap(_lookAt, camera._lookAt);

  return *this;
}

BaseCamera::~BaseCamera() noexcept {}

void BaseCamera::setPosition(float eyeX, float eyeY, float eyeZ) noexcept {
  _pos = glm::vec3{eyeX, eyeY, eyeZ};
}

void BaseCamera::lookAt(float centerX, float centerY, float centerZ) noexcept {
  _lookAt  = glm::vec3{centerX, centerY, centerZ};
  _forward = glm::normalize(_lookAt - _pos);
  _right   = glm::cross(_forward, glm::vec3{0.0f, 1.0f, 0.0f});
  _up      = glm::cross(_right, _forward);
}

const glm::vec3 &BaseCamera::getPosition() const noexcept { return _pos; }

const glm::vec3 &BaseCamera::getForwardDirection() const noexcept {
  return _forward;
}

const glm::vec3 &BaseCamera::getRightDirection() const noexcept {
  return _right;
}

const glm::vec3 &BaseCamera::getUpDirection() const noexcept { return _up; }

const glm::vec3 &BaseCamera::getLookAtCenter() const noexcept {
  return _lookAt;
}

glm::mat4 BaseCamera::getViewMatrix() const noexcept {
  return glm::lookAt(_pos, _lookAt, _up);
}

// OrthographicCamera

OrthographicCamera::OrthographicCamera() noexcept
    : BaseCamera{},
      _leftBorder{-1.0f},
      _rightBorder{1.0f},
      _bottomBorder{-1.0f},
      _topBorder{1.0f},
      _nearPlane{0.1f},
      _farPlane{100.0f} {}

OrthographicCamera::OrthographicCamera(const BaseCamera &camera) noexcept
    : BaseCamera{camera} {}

OrthographicCamera &OrthographicCamera::operator=(
    const BaseCamera &camera) noexcept {
  this->BaseCamera::operator=(camera);

  return *this;
}

OrthographicCamera::OrthographicCamera(
    const OrthographicCamera &camera) noexcept
    : BaseCamera{dynamic_cast<const BaseCamera &>(camera)},
      _leftBorder{camera._leftBorder},
      _rightBorder{camera._rightBorder},
      _bottomBorder{camera._bottomBorder},
      _topBorder{camera._topBorder},
      _nearPlane{camera._nearPlane},
      _farPlane{camera._farPlane} {}

OrthographicCamera &OrthographicCamera::operator=(
    const OrthographicCamera &camera) noexcept {
  this->BaseCamera::operator=(camera);

  _leftBorder   = camera._leftBorder;
  _rightBorder  = camera._rightBorder;
  _bottomBorder = camera._bottomBorder;
  _topBorder    = camera._topBorder;
  _nearPlane    = camera._nearPlane;
  _farPlane     = camera._farPlane;

  return *this;
}

OrthographicCamera::OrthographicCamera(OrthographicCamera &&camera) noexcept
    : BaseCamera{dynamic_cast<BaseCamera &&>(camera)},
      _leftBorder{std::exchange(camera._leftBorder, 0.0f)},
      _rightBorder{std::exchange(camera._rightBorder, 0.0f)},
      _bottomBorder{std::exchange(camera._bottomBorder, 0.0f)},
      _topBorder{std::exchange(camera._topBorder, 0.0f)},
      _nearPlane{std::exchange(camera._nearPlane, 0.0f)},
      _farPlane{std::exchange(camera._farPlane, 0.0f)} {}

OrthographicCamera &OrthographicCamera::operator=(
    OrthographicCamera &&camera) noexcept {
  this->BaseCamera::operator=(camera);

  std::swap(_leftBorder, camera._leftBorder);
  std::swap(_rightBorder, camera._rightBorder);
  std::swap(_bottomBorder, camera._bottomBorder);
  std::swap(_topBorder, camera._topBorder);
  std::swap(_nearPlane, camera._nearPlane);
  std::swap(_farPlane, camera._farPlane);

  return *this;
}

OrthographicCamera::~OrthographicCamera() noexcept {}

void OrthographicCamera::setProjection(float leftBorder, float rightBorder,
                                       float bottomBorder, float topBorder,
                                       float nearPlane, float farPlane) {
  _leftBorder   = leftBorder;
  _rightBorder  = rightBorder;
  _bottomBorder = bottomBorder;
  _topBorder    = topBorder;
  _nearPlane    = nearPlane;
  _farPlane     = farPlane;
}

glm::mat4 OrthographicCamera::getProjMatrix() const noexcept {
  return glm::ortho(_leftBorder, _rightBorder, _bottomBorder, _topBorder,
                    _nearPlane, _farPlane);
}

// PerspectiveCamera

PerspectiveCamera::PerspectiveCamera() noexcept
    : BaseCamera{},
      _verticalFOV{glm::radians(45.0f)},
      _aspectRatio{4.0f / 3.0f},
      _nearPlane{0.1f},
      _farPlane{100.0f} {}

PerspectiveCamera::PerspectiveCamera(const BaseCamera &camera) noexcept
    : BaseCamera{camera} {}

PerspectiveCamera &PerspectiveCamera::operator=(
    const BaseCamera &camera) noexcept {
  this->BaseCamera::operator=(camera);

  return *this;
}

PerspectiveCamera::PerspectiveCamera(const PerspectiveCamera &camera) noexcept
    : BaseCamera{dynamic_cast<const BaseCamera &>(camera)},
      _verticalFOV{camera._verticalFOV},
      _aspectRatio{camera._aspectRatio},
      _nearPlane{camera._nearPlane},
      _farPlane{camera._farPlane} {}

PerspectiveCamera &PerspectiveCamera::operator=(
    const PerspectiveCamera &camera) noexcept {
  this->BaseCamera::operator=(camera);

  _verticalFOV = camera._verticalFOV;
  _aspectRatio = camera._aspectRatio;
  _nearPlane   = camera._nearPlane;
  _farPlane    = camera._farPlane;

  return *this;
}

PerspectiveCamera::PerspectiveCamera(PerspectiveCamera &&camera) noexcept
    : BaseCamera{dynamic_cast<BaseCamera &&>(camera)},
      _verticalFOV{std::exchange(camera._verticalFOV, 0.0f)},
      _aspectRatio{std::exchange(camera._aspectRatio, 0.0f)},
      _nearPlane{std::exchange(camera._nearPlane, 0.0f)},
      _farPlane{std::exchange(camera._farPlane, 0.0f)} {}

PerspectiveCamera &PerspectiveCamera::operator=(
    PerspectiveCamera &&camera) noexcept {
  this->BaseCamera::operator=(camera);

  std::swap(_verticalFOV, camera._verticalFOV);
  std::swap(_aspectRatio, camera._aspectRatio);
  std::swap(_nearPlane, camera._nearPlane);
  std::swap(_farPlane, camera._farPlane);

  return *this;
}

PerspectiveCamera::~PerspectiveCamera() noexcept {}

void PerspectiveCamera::setProjection(float verticalFOV, float aspectRatio,
                                      float nearPlane, float farPlane) {
  _verticalFOV = verticalFOV;
  _aspectRatio = aspectRatio;
  _nearPlane   = nearPlane;
  _farPlane    = farPlane;
}

glm::mat4 PerspectiveCamera::getProjMatrix() const noexcept {
  return glm::perspective(_verticalFOV, _aspectRatio, _nearPlane, _farPlane);
}
