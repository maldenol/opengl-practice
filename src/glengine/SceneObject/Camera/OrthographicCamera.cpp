// Header file
#include "./OrthographicCamera.hpp"

// STD
#include <utility>

// GLM
#include <glm/gtc/matrix_transform.hpp>

using namespace glengine;

// Constructors, assignment operators and destructor

// Default constructor
OrthographicCamera::OrthographicCamera() noexcept : BaseCamera{} { recalculateProjectionMatrix(); }

// Copy constructor
OrthographicCamera::OrthographicCamera(const BaseCamera &camera) noexcept : BaseCamera{camera} {
  recalculateProjectionMatrix();
}

// Copy assignment operator
OrthographicCamera &OrthographicCamera::operator=(const BaseCamera &camera) noexcept {
  this->BaseCamera::operator=(camera);

  recalculateProjectionMatrix();

  return *this;
}

// Copy constructor
OrthographicCamera::OrthographicCamera(const OrthographicCamera &camera) noexcept
    : BaseCamera{dynamic_cast<const BaseCamera &>(camera)},
      _leftBorder{camera._leftBorder},
      _rightBorder{camera._rightBorder},
      _bottomBorder{camera._bottomBorder},
      _topBorder{camera._topBorder},
      _nearPlane{camera._nearPlane},
      _farPlane{camera._farPlane} {}

// Copy assignment operator
OrthographicCamera &OrthographicCamera::operator=(const OrthographicCamera &camera) noexcept {
  this->BaseCamera::operator=(dynamic_cast<const BaseCamera &>(camera));

  _leftBorder   = camera._leftBorder;
  _rightBorder  = camera._rightBorder;
  _bottomBorder = camera._bottomBorder;
  _topBorder    = camera._topBorder;
  _nearPlane    = camera._nearPlane;
  _farPlane     = camera._farPlane;

  return *this;
}

// Move constructor
OrthographicCamera::OrthographicCamera(OrthographicCamera &&camera) noexcept
    : BaseCamera{dynamic_cast<BaseCamera &&>(camera)},
      _leftBorder{std::exchange(camera._leftBorder, 0.0f)},
      _rightBorder{std::exchange(camera._rightBorder, 0.0f)},
      _bottomBorder{std::exchange(camera._bottomBorder, 0.0f)},
      _topBorder{std::exchange(camera._topBorder, 0.0f)},
      _nearPlane{std::exchange(camera._nearPlane, 0.0f)},
      _farPlane{std::exchange(camera._farPlane, 0.0f)} {}

// Move assignment operator
OrthographicCamera &OrthographicCamera::operator=(OrthographicCamera &&camera) noexcept {
  this->BaseCamera::operator=(dynamic_cast<BaseCamera &&>(camera));

  std::swap(_leftBorder, camera._leftBorder);
  std::swap(_rightBorder, camera._rightBorder);
  std::swap(_bottomBorder, camera._bottomBorder);
  std::swap(_topBorder, camera._topBorder);
  std::swap(_nearPlane, camera._nearPlane);
  std::swap(_farPlane, camera._farPlane);

  return *this;
}

// Destructor
OrthographicCamera::~OrthographicCamera() noexcept {}

// Setters

void OrthographicCamera::setLeftBorder(float leftBorder) noexcept {
  _leftBorder = leftBorder;

  recalculateProjectionMatrix();
}

void OrthographicCamera::setRightBorder(float rightBorder) noexcept {
  _rightBorder = rightBorder;

  recalculateProjectionMatrix();
}

void OrthographicCamera::setBottomBorder(float bottomBorder) noexcept {
  _bottomBorder = bottomBorder;

  recalculateProjectionMatrix();
}

void OrthographicCamera::setTopBorder(float topBorder) noexcept {
  _topBorder = topBorder;

  recalculateProjectionMatrix();
}

void OrthographicCamera::setNearPlane(float nearPlane) noexcept {
  _nearPlane = nearPlane;

  recalculateProjectionMatrix();
}

void OrthographicCamera::setFarPlane(float farPlane) noexcept {
  _farPlane = farPlane;

  recalculateProjectionMatrix();
}

// Getters

float OrthographicCamera::getLeftBorder() const noexcept { return _leftBorder; }

float &OrthographicCamera::getLeftBorder() noexcept { return _leftBorder; }

float OrthographicCamera::getRightBorder() const noexcept { return _rightBorder; }

float &OrthographicCamera::getRightBorder() noexcept { return _rightBorder; }

float OrthographicCamera::getBottomBorder() const noexcept { return _bottomBorder; }

float &OrthographicCamera::getBottomBorder() noexcept { return _bottomBorder; }

float OrthographicCamera::getTopBorder() const noexcept { return _topBorder; }

float &OrthographicCamera::getTopBorder() noexcept { return _topBorder; }

float OrthographicCamera::getNearPlane() const noexcept { return _nearPlane; }

float &OrthographicCamera::getNearPlane() noexcept { return _nearPlane; }

float OrthographicCamera::getFarPlane() const noexcept { return _farPlane; }

float &OrthographicCamera::getFarPlane() noexcept { return _farPlane; }

// Other member functions

void OrthographicCamera::recalculateProjectionMatrix() noexcept {
  _projectionMatrix =
      glm::ortho(_leftBorder, _rightBorder, _bottomBorder, _topBorder, _nearPlane, _farPlane);
}
