// Header file
#include "./OrthographicCamera.hpp"

// STD
#include <utility>

// GLM
#include <glm/gtc/matrix_transform.hpp>

// Global constants
static constexpr float kInitLeftBorder   = -1.0f;
static constexpr float kInitRightBorder  = 1.0f;
static constexpr float kInitBottomBorder = -1.0f;
static constexpr float kInitTopBorder    = 1.0f;
static constexpr float kInitNearPlane    = 0.1f;
static constexpr float kInitFarPlane     = 100.0f;

using namespace glengine;

// Constructors, assignment operators and destructor

// Default constructor
OrthographicCamera::OrthographicCamera() noexcept
    : BaseCamera{},
      _leftBorder{kInitLeftBorder},
      _rightBorder{kInitRightBorder},
      _bottomBorder{kInitBottomBorder},
      _topBorder{kInitTopBorder},
      _nearPlane{kInitNearPlane},
      _farPlane{kInitFarPlane} {}

// Copy constructor
OrthographicCamera::OrthographicCamera(const BaseCamera &camera) noexcept
    : BaseCamera{camera},
      _leftBorder{kInitLeftBorder},
      _rightBorder{kInitRightBorder},
      _bottomBorder{kInitBottomBorder},
      _topBorder{kInitTopBorder},
      _nearPlane{kInitNearPlane},
      _farPlane{kInitFarPlane} {}

// Copy assignment operator
OrthographicCamera &OrthographicCamera::operator=(const BaseCamera &camera) noexcept {
  this->BaseCamera::operator=(camera);

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

// Other member functions

void OrthographicCamera::setProjectionAttributes(float leftBorder, float rightBorder,
                                                 float bottomBorder, float topBorder,
                                                 float nearPlane, float farPlane) noexcept {
  _leftBorder   = leftBorder;
  _rightBorder  = rightBorder;
  _bottomBorder = bottomBorder;
  _topBorder    = topBorder;
  _nearPlane    = nearPlane;
  _farPlane     = farPlane;
}

void OrthographicCamera::getProjectionAttributes(float &leftBorder, float &rightBorder,
                                                 float &bottomBorder, float &topBorder,
                                                 float &nearPlane, float &farPlane) const noexcept {
  leftBorder   = _leftBorder;
  rightBorder  = _rightBorder;
  bottomBorder = _bottomBorder;
  topBorder    = _topBorder;
  nearPlane    = _nearPlane;
  farPlane     = _farPlane;
}

glm::mat4 OrthographicCamera::getProjectionMatrix() const noexcept {
  return glm::ortho(_leftBorder, _rightBorder, _bottomBorder, _topBorder, _nearPlane, _farPlane);
}