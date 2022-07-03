// Header file
#include "./PerspectiveCamera.hpp"

// STD
#include <utility>

// GLM
#include <glm/gtc/matrix_transform.hpp>

// Global constants
static constexpr float kInitVerticalFov = glm::radians(60.0f);
static constexpr float kInitAspectRatio = 4.0f / 3.0f;
static constexpr float kInitNearPlane   = 0.1f;
static constexpr float kInitFarPlane    = 100.0f;

glengine::PerspectiveCamera::PerspectiveCamera() noexcept
    : BaseCamera{},
      _verticalFOV{kInitVerticalFov},
      _aspectRatio{kInitAspectRatio},
      _nearPlane{kInitNearPlane},
      _farPlane{kInitFarPlane} {}

glengine::PerspectiveCamera::PerspectiveCamera(const BaseCamera &camera) noexcept
    : BaseCamera{dynamic_cast<const BaseCamera &>(camera)},
      _verticalFOV{kInitVerticalFov},
      _aspectRatio{kInitAspectRatio},
      _nearPlane{kInitNearPlane},
      _farPlane{kInitFarPlane} {}

glengine::PerspectiveCamera &glengine::PerspectiveCamera::operator=(
    const BaseCamera &camera) noexcept {
  this->BaseCamera::operator=(dynamic_cast<const BaseCamera &>(camera));

  return *this;
}

glengine::PerspectiveCamera::PerspectiveCamera(const PerspectiveCamera &camera) noexcept
    : BaseCamera{dynamic_cast<const BaseCamera &>(camera)},
      _verticalFOV{camera._verticalFOV},
      _aspectRatio{camera._aspectRatio},
      _nearPlane{camera._nearPlane},
      _farPlane{camera._farPlane} {}

glengine::PerspectiveCamera &glengine::PerspectiveCamera::operator=(
    const PerspectiveCamera &camera) noexcept {
  this->BaseCamera::operator=(dynamic_cast<const BaseCamera &>(camera));

  _verticalFOV = camera._verticalFOV;
  _aspectRatio = camera._aspectRatio;
  _nearPlane   = camera._nearPlane;
  _farPlane    = camera._farPlane;

  return *this;
}

glengine::PerspectiveCamera::PerspectiveCamera(PerspectiveCamera &&camera) noexcept
    : BaseCamera{dynamic_cast<BaseCamera &&>(camera)},
      _verticalFOV{std::exchange(camera._verticalFOV, 0.0f)},
      _aspectRatio{std::exchange(camera._aspectRatio, 0.0f)},
      _nearPlane{std::exchange(camera._nearPlane, 0.0f)},
      _farPlane{std::exchange(camera._farPlane, 0.0f)} {}

glengine::PerspectiveCamera &glengine::PerspectiveCamera::operator=(
    PerspectiveCamera &&camera) noexcept {
  this->BaseCamera::operator=(dynamic_cast<BaseCamera &&>(camera));

  std::swap(_verticalFOV, camera._verticalFOV);
  std::swap(_aspectRatio, camera._aspectRatio);
  std::swap(_nearPlane, camera._nearPlane);
  std::swap(_farPlane, camera._farPlane);

  return *this;
}

glengine::PerspectiveCamera::~PerspectiveCamera() noexcept {}

void glengine::PerspectiveCamera::setProjectionAttributes(float verticalFOV, float aspectRatio,
                                                          float nearPlane,
                                                          float farPlane) noexcept {
  _verticalFOV = verticalFOV;
  _aspectRatio = aspectRatio;
  _nearPlane   = nearPlane;
  _farPlane    = farPlane;
}

void glengine::PerspectiveCamera::getProjectionAttributes(float &verticalFOV, float &aspectRatio,
                                                          float &nearPlane,
                                                          float &farPlane) const noexcept {
  verticalFOV = _verticalFOV;
  aspectRatio = _aspectRatio;
  nearPlane   = _nearPlane;
  farPlane    = _farPlane;
}

glm::mat4 glengine::PerspectiveCamera::getProjectionMatrix() const noexcept {
  return glm::perspective(_verticalFOV, _aspectRatio, _nearPlane, _farPlane);
}
