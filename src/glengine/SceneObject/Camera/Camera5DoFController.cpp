// Header file
#include "./Camera5DoFController.hpp"

// STD
#include <utility>

// GLM
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

// Global constants
static constexpr float kTwoPi = 6.28318530f;

float angleIntoBounds(float angle, float angleLimitMin, float angleLimitMax);

using namespace glengine;

// Constructors, assignment operators and destructor

// Default constructor
Camera5DoFController::Camera5DoFController() noexcept {}

// Parameterized constructor
Camera5DoFController::Camera5DoFController(BaseCamera *camera) noexcept
    : Camera3DoFController{camera} {
  updateLook();
}

// Copy constructor (base class)
Camera5DoFController::Camera5DoFController(const Camera3DoFController &cameraController) noexcept
    : Camera3DoFController{cameraController} {
  updateLook();
}

// Copy assignment operator (base class)
Camera5DoFController &Camera5DoFController::operator=(
    const Camera3DoFController &cameraController) noexcept {
  this->Camera3DoFController::operator=(cameraController);

  updateLook();

  return *this;
}

// Copy constructor
Camera5DoFController::Camera5DoFController(const Camera5DoFController &cameraController) noexcept
    : Camera3DoFController{dynamic_cast<const Camera3DoFController &>(cameraController)},
      _angleUp{cameraController._angleUp},
      _angleRight{cameraController._angleRight},
      _angleUpLimitMin{cameraController._angleUpLimitMin},
      _angleUpLimitMax{cameraController._angleUpLimitMax},
      _angleRightLimitMin{cameraController._angleRightLimitMin},
      _angleRightLimitMax{cameraController._angleRightLimitMax},
      _baseLookDirection{cameraController._baseLookDirection} {}

// Copy assignment operator
Camera5DoFController &Camera5DoFController::operator=(
    const Camera5DoFController &cameraController) noexcept {
  this->Camera3DoFController::operator=(
      dynamic_cast<const Camera3DoFController &>(cameraController));

  _angleUp            = cameraController._angleUp;
  _angleRight         = cameraController._angleRight;
  _angleUpLimitMin    = cameraController._angleUpLimitMin;
  _angleUpLimitMax    = cameraController._angleUpLimitMax;
  _angleRightLimitMin = cameraController._angleRightLimitMin;
  _angleRightLimitMax = cameraController._angleRightLimitMax;

  return *this;
}

// Move constructor
Camera5DoFController::Camera5DoFController(Camera5DoFController &&cameraController) noexcept
    : Camera3DoFController{dynamic_cast<Camera3DoFController &&>(cameraController)},
      _angleUp{std::exchange(cameraController._angleUp, 0.0f)},
      _angleRight{std::exchange(cameraController._angleRight, 0.0f)},
      _angleUpLimitMin{std::exchange(cameraController._angleUpLimitMin, 0.0f)},
      _angleUpLimitMax{std::exchange(cameraController._angleUpLimitMax, 0.0f)},
      _angleRightLimitMin{std::exchange(cameraController._angleRightLimitMin, 0.0f)},
      _angleRightLimitMax{std::exchange(cameraController._angleRightLimitMax, 0.0f)} {}

// Move assignment operator
Camera5DoFController &Camera5DoFController::operator=(
    Camera5DoFController &&cameraController) noexcept {
  this->Camera3DoFController::operator=(dynamic_cast<Camera3DoFController &&>(cameraController));

  std::swap(_angleUp, cameraController._angleUp);
  std::swap(_angleRight, cameraController._angleRight);
  std::swap(_angleUpLimitMin, cameraController._angleUpLimitMin);
  std::swap(_angleUpLimitMax, cameraController._angleUpLimitMax);
  std::swap(_angleRightLimitMin, cameraController._angleRightLimitMin);
  std::swap(_angleRightLimitMax, cameraController._angleRightLimitMax);

  return *this;
}

// Destructor
Camera5DoFController::~Camera5DoFController() noexcept {}

// Setters

void Camera5DoFController::setCamera(BaseCamera *camera) noexcept {
  _camera = camera;
  updateLook();
}

// Other member functions

void Camera5DoFController::updateLook() noexcept {
  glm::vec3 look                          = _camera->_forward;
  float     absoluteAngleBetweenLookAndUp = glm::angle(look, _camera->_worldUp);
  float     rotationAngle                 = absoluteAngleBetweenLookAndUp - glm::radians(90.0f);
  glm::vec3 rotationAxis{glm::cross(look, _camera->_worldUp)};
  _baseLookDirection = glm::rotate(look, rotationAngle, rotationAxis);

  setAngles(0.0f, -rotationAngle);
}

void Camera5DoFController::moveUp(float distance) noexcept {
  _camera->_pos += _camera->_worldUp * distance;
}

void Camera5DoFController::setAngles(float angleUp, float angleRight) noexcept {
  _angleUp    = angleUp;
  _angleUp    = angleIntoBounds(_angleUp, _angleUpLimitMin, _angleUpLimitMax);
  _angleRight = angleRight;
  _angleRight = angleIntoBounds(_angleRight, _angleRightLimitMin, _angleRightLimitMax);
}

void Camera5DoFController::getAngles(float &angleUp, float &angleRight) const noexcept {
  angleUp    = _angleUp;
  angleRight = _angleRight;
}

void Camera5DoFController::setAngleLimits(float angleUpLimitMin, float angleUpLimitMax,
                                          float angleRightLimitMin,
                                          float angleRightLimitMax) noexcept {
  _angleUpLimitMin    = angleUpLimitMin;
  _angleUpLimitMax    = angleUpLimitMax;
  _angleRightLimitMin = angleRightLimitMin;
  _angleRightLimitMax = angleRightLimitMax;
}

void Camera5DoFController::getAngleLimits(float &angleUpLimitMin, float &angleUpLimitMax,
                                          float &angleRightLimitMin,
                                          float &angleRightLimitMax) const noexcept {
  angleUpLimitMin    = _angleUpLimitMin;
  angleUpLimitMax    = _angleUpLimitMax;
  angleRightLimitMin = _angleRightLimitMin;
  angleRightLimitMax = _angleRightLimitMax;
}

void Camera5DoFController::addAngles(float angleUp, float angleRight) noexcept {
  _angleUp += angleUp;
  _angleUp = angleIntoBounds(_angleUp, _angleUpLimitMin, _angleUpLimitMax);
  _angleRight += angleRight;
  _angleRight = angleIntoBounds(_angleRight, _angleRightLimitMin, _angleRightLimitMax);

  glm::vec3 look{_baseLookDirection};
  look = glm::rotate(look, _angleRight, _camera->_right);
  look = glm::rotate(look, _angleUp, _camera->_worldUp);
  _camera->look(look);
}

float angleIntoBounds(float angle, float angleLimitMin, float angleLimitMax) {
  if (angleLimitMin == 0.0f && angleLimitMax == 0.0f) {
    angle -= kTwoPi * static_cast<float>(static_cast<int>(angle / kTwoPi));
  } else if (angle < angleLimitMin) {
    angle = angleLimitMin;
  } else if (angle > angleLimitMax) {
    angle = angleLimitMax;
  }

  return angle;
}