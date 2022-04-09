// Header file
#include <glservice/camera/PerspectiveCamera.hpp>

// STD
#include <utility>

glservice::PerspectiveCamera::PerspectiveCamera() noexcept
    : BaseCamera{},
      _verticalFOV{glm::radians(45.0f)},
      _aspectRatio{4.0f / 3.0f},
      _nearPlane{0.1f},
      _farPlane{100.0f} {}

glservice::PerspectiveCamera::PerspectiveCamera(
    const BaseCamera &camera) noexcept
    : BaseCamera{camera} {}

glservice::PerspectiveCamera &glservice::PerspectiveCamera::operator=(
    const BaseCamera &camera) noexcept {
  this->BaseCamera::operator=(camera);

  return *this;
}

glservice::PerspectiveCamera::PerspectiveCamera(
    const PerspectiveCamera &camera) noexcept
    : BaseCamera{dynamic_cast<const BaseCamera &>(camera)},
      _verticalFOV{camera._verticalFOV},
      _aspectRatio{camera._aspectRatio},
      _nearPlane{camera._nearPlane},
      _farPlane{camera._farPlane} {}

glservice::PerspectiveCamera &glservice::PerspectiveCamera::operator=(
    const PerspectiveCamera &camera) noexcept {
  this->BaseCamera::operator=(camera);

  _verticalFOV = camera._verticalFOV;
  _aspectRatio = camera._aspectRatio;
  _nearPlane   = camera._nearPlane;
  _farPlane    = camera._farPlane;

  return *this;
}

glservice::PerspectiveCamera::PerspectiveCamera(
    PerspectiveCamera &&camera) noexcept
    : BaseCamera{dynamic_cast<BaseCamera &&>(camera)},
      _verticalFOV{std::exchange(camera._verticalFOV, 0.0f)},
      _aspectRatio{std::exchange(camera._aspectRatio, 0.0f)},
      _nearPlane{std::exchange(camera._nearPlane, 0.0f)},
      _farPlane{std::exchange(camera._farPlane, 0.0f)} {}

glservice::PerspectiveCamera &glservice::PerspectiveCamera::operator=(
    PerspectiveCamera &&camera) noexcept {
  this->BaseCamera::operator=(camera);

  std::swap(_verticalFOV, camera._verticalFOV);
  std::swap(_aspectRatio, camera._aspectRatio);
  std::swap(_nearPlane, camera._nearPlane);
  std::swap(_farPlane, camera._farPlane);

  return *this;
}

glservice::PerspectiveCamera::~PerspectiveCamera() noexcept {}

void glservice::PerspectiveCamera::setProjection(float verticalFOV,
                                                 float aspectRatio,
                                                 float nearPlane,
                                                 float farPlane) {
  _verticalFOV = verticalFOV;
  _aspectRatio = aspectRatio;
  _nearPlane   = nearPlane;
  _farPlane    = farPlane;
}

glm::mat4 glservice::PerspectiveCamera::getProjMatrix() const noexcept {
  return glm::perspective(_verticalFOV, _aspectRatio, _nearPlane, _farPlane);
}
