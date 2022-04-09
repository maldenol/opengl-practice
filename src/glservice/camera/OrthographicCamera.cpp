// Header file
#include <glservice/camera/OrthographicCamera.hpp>

// STD
#include <utility>

glservice::OrthographicCamera::OrthographicCamera() noexcept
    : BaseCamera{},
      _leftBorder{-1.0f},
      _rightBorder{1.0f},
      _bottomBorder{-1.0f},
      _topBorder{1.0f},
      _nearPlane{0.1f},
      _farPlane{100.0f} {}

glservice::OrthographicCamera::OrthographicCamera(
    const BaseCamera &camera) noexcept
    : BaseCamera{camera} {}

glservice::OrthographicCamera &glservice::OrthographicCamera::operator=(
    const BaseCamera &camera) noexcept {
  this->BaseCamera::operator=(camera);

  return *this;
}

glservice::OrthographicCamera::OrthographicCamera(
    const OrthographicCamera &camera) noexcept
    : BaseCamera{dynamic_cast<const BaseCamera &>(camera)},
      _leftBorder{camera._leftBorder},
      _rightBorder{camera._rightBorder},
      _bottomBorder{camera._bottomBorder},
      _topBorder{camera._topBorder},
      _nearPlane{camera._nearPlane},
      _farPlane{camera._farPlane} {}

glservice::OrthographicCamera &glservice::OrthographicCamera::operator=(
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

glservice::OrthographicCamera::OrthographicCamera(
    OrthographicCamera &&camera) noexcept
    : BaseCamera{dynamic_cast<BaseCamera &&>(camera)},
      _leftBorder{std::exchange(camera._leftBorder, 0.0f)},
      _rightBorder{std::exchange(camera._rightBorder, 0.0f)},
      _bottomBorder{std::exchange(camera._bottomBorder, 0.0f)},
      _topBorder{std::exchange(camera._topBorder, 0.0f)},
      _nearPlane{std::exchange(camera._nearPlane, 0.0f)},
      _farPlane{std::exchange(camera._farPlane, 0.0f)} {}

glservice::OrthographicCamera &glservice::OrthographicCamera::operator=(
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

glservice::OrthographicCamera::~OrthographicCamera() noexcept {}

void glservice::OrthographicCamera::setProjectionAttributes(
    float leftBorder, float rightBorder, float bottomBorder, float topBorder,
    float nearPlane, float farPlane) noexcept {
  _leftBorder   = leftBorder;
  _rightBorder  = rightBorder;
  _bottomBorder = bottomBorder;
  _topBorder    = topBorder;
  _nearPlane    = nearPlane;
  _farPlane     = farPlane;
}

void glservice::OrthographicCamera::getProjectionAttributes(
    float &leftBorder, float &rightBorder, float &bottomBorder,
    float &topBorder, float &nearPlane, float &farPlane) const noexcept {
  leftBorder   = _leftBorder;
  rightBorder  = _rightBorder;
  bottomBorder = _bottomBorder;
  topBorder    = _topBorder;
  nearPlane    = _nearPlane;
  farPlane     = _farPlane;
}

glm::mat4 glservice::OrthographicCamera::getProjectionMatrix() const noexcept {
  return glm::ortho(_leftBorder, _rightBorder, _bottomBorder, _topBorder,
                    _nearPlane, _farPlane);
}
