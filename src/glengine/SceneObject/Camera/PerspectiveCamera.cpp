// Header file
#include "./PerspectiveCamera.hpp"

// STD
#include <utility>

// GLM
#include <glm/gtc/matrix_transform.hpp>

using namespace glengine;

// Constructors, assignment operators and destructor

// Default constructor
PerspectiveCamera::PerspectiveCamera() noexcept : BaseCamera{} { recalculateProjectionMatrix(); }

// Copy constructor (base class)
PerspectiveCamera::PerspectiveCamera(const BaseCamera &camera) noexcept : BaseCamera{camera} {
  recalculateProjectionMatrix();
}

// Copy assignment operator (base class)
PerspectiveCamera &PerspectiveCamera::operator=(const BaseCamera &camera) noexcept {
  this->BaseCamera::operator=(camera);

  recalculateProjectionMatrix();

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

// Setters

void PerspectiveCamera::setVerticalFOV(float verticalVOF) noexcept {
  _verticalFOV = verticalVOF;

  recalculateProjectionMatrix();
}

void PerspectiveCamera::setAspectRatio(float aspectRatio) noexcept {
  _aspectRatio = aspectRatio;

  recalculateProjectionMatrix();
}

void PerspectiveCamera::setNearPlane(float nearPlane) noexcept {
  _nearPlane = nearPlane;

  recalculateProjectionMatrix();
}

void PerspectiveCamera::setFarPlane(float farPlane) noexcept {
  _farPlane = farPlane;

  recalculateProjectionMatrix();
}

// Getters

float PerspectiveCamera::getVerticalFOV() const noexcept { return _verticalFOV; }

float &PerspectiveCamera::getVerticalFOV() noexcept { return _verticalFOV; }

float PerspectiveCamera::getAspectRatio() const noexcept { return _aspectRatio; }

float &PerspectiveCamera::getAspectRatio() noexcept { return _aspectRatio; }

float PerspectiveCamera::getNearPlane() const noexcept { return _nearPlane; }

float &PerspectiveCamera::getNearPlane() noexcept { return _nearPlane; }

float PerspectiveCamera::getFarPlane() const noexcept { return _farPlane; }

float &PerspectiveCamera::getFarPlane() noexcept { return _farPlane; }

// Other member functions

void PerspectiveCamera::recalculateProjectionMatrix() noexcept {
  _projectionMatrix = glm::perspective(_verticalFOV, _aspectRatio, _nearPlane, _farPlane);
}
