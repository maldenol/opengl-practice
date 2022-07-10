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

using namespace glengine;

// Constructors, assignment operators and destructor

// Default constructor
PerspectiveCamera::PerspectiveCamera() noexcept
    : BaseCamera{},
      _verticalFOV{kInitVerticalFov},
      _aspectRatio{kInitAspectRatio},
      _nearPlane{kInitNearPlane},
      _farPlane{kInitFarPlane} {}

// Copy constructor (base class)
PerspectiveCamera::PerspectiveCamera(const BaseCamera &camera) noexcept
    : BaseCamera{camera},
      _verticalFOV{kInitVerticalFov},
      _aspectRatio{kInitAspectRatio},
      _nearPlane{kInitNearPlane},
      _farPlane{kInitFarPlane} {}

// Copy assignment operator (base class)
PerspectiveCamera &PerspectiveCamera::operator=(const BaseCamera &camera) noexcept {
  this->BaseCamera::operator=(camera);

  return *this;
}

// Copy constructor
PerspectiveCamera::PerspectiveCamera(const PerspectiveCamera &camera) noexcept
    : BaseCamera{dynamic_cast<const BaseCamera &>(camera)},
      _verticalFOV{camera._verticalFOV},
      _aspectRatio{camera._aspectRatio},
      _nearPlane{camera._nearPlane},
      _farPlane{camera._farPlane} {}

// Copy assignment operator
PerspectiveCamera &PerspectiveCamera::operator=(const PerspectiveCamera &camera) noexcept {
  this->BaseCamera::operator=(dynamic_cast<const BaseCamera &>(camera));

  _verticalFOV = camera._verticalFOV;
  _aspectRatio = camera._aspectRatio;
  _nearPlane   = camera._nearPlane;
  _farPlane    = camera._farPlane;

  return *this;
}

// Move constructor
PerspectiveCamera::PerspectiveCamera(PerspectiveCamera &&camera) noexcept
    : BaseCamera{dynamic_cast<BaseCamera &&>(camera)},
      _verticalFOV{std::exchange(camera._verticalFOV, 0.0f)},
      _aspectRatio{std::exchange(camera._aspectRatio, 0.0f)},
      _nearPlane{std::exchange(camera._nearPlane, 0.0f)},
      _farPlane{std::exchange(camera._farPlane, 0.0f)} {}

// Move assignment operator
PerspectiveCamera &PerspectiveCamera::operator=(PerspectiveCamera &&camera) noexcept {
  this->BaseCamera::operator=(dynamic_cast<BaseCamera &&>(camera));

  std::swap(_verticalFOV, camera._verticalFOV);
  std::swap(_aspectRatio, camera._aspectRatio);
  std::swap(_nearPlane, camera._nearPlane);
  std::swap(_farPlane, camera._farPlane);

  return *this;
}

// Destructor
PerspectiveCamera::~PerspectiveCamera() noexcept {}

// Other member functions

void PerspectiveCamera::setProjectionAttributes(float verticalFOV, float aspectRatio,
                                                float nearPlane, float farPlane) noexcept {
  _verticalFOV = verticalFOV;
  _aspectRatio = aspectRatio;
  _nearPlane   = nearPlane;
  _farPlane    = farPlane;
}

void PerspectiveCamera::getProjectionAttributes(float &verticalFOV, float &aspectRatio,
                                                float &nearPlane, float &farPlane) const noexcept {
  verticalFOV = _verticalFOV;
  aspectRatio = _aspectRatio;
  nearPlane   = _nearPlane;
  farPlane    = _farPlane;
}

glm::mat4 PerspectiveCamera::getProjectionMatrix() const noexcept {
  return glm::perspective(_verticalFOV, _aspectRatio, _nearPlane, _farPlane);
}