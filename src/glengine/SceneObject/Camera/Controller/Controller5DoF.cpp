// Header file
#include "./Controller5DoF.hpp"

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
Controller5DoF::Controller5DoF() noexcept {}

// Parameterized constructor
Controller5DoF::Controller5DoF(BaseCamera *camera) noexcept : Controller3DoF{camera} {
  updateLook();
}

// Copy constructor (base class)
Controller5DoF::Controller5DoF(const Controller3DoF &cameraController) noexcept
    : Controller3DoF{cameraController} {
  updateLook();
}

// Copy assignment operator (base class)
Controller5DoF &Controller5DoF::operator=(const Controller3DoF &cameraController) noexcept {
  this->Controller3DoF::operator=(cameraController);

  updateLook();

  return *this;
}

// Copy constructor
Controller5DoF::Controller5DoF(const Controller5DoF &cameraController) noexcept
    : Controller3DoF{dynamic_cast<const Controller3DoF &>(cameraController)},
      _angleUp{cameraController._angleUp},
      _angleRight{cameraController._angleRight},
      _angleUpLimitMin{cameraController._angleUpLimitMin},
      _angleUpLimitMax{cameraController._angleUpLimitMax},
      _angleRightLimitMin{cameraController._angleRightLimitMin},
      _angleRightLimitMax{cameraController._angleRightLimitMax},
      _baseLookDirection{cameraController._baseLookDirection} {}

// Copy assignment operator
Controller5DoF &Controller5DoF::operator=(const Controller5DoF &cameraController) noexcept {
  this->Controller3DoF::operator=(dynamic_cast<const Controller3DoF &>(cameraController));

  _angleUp            = cameraController._angleUp;
  _angleRight         = cameraController._angleRight;
  _angleUpLimitMin    = cameraController._angleUpLimitMin;
  _angleUpLimitMax    = cameraController._angleUpLimitMax;
  _angleRightLimitMin = cameraController._angleRightLimitMin;
  _angleRightLimitMax = cameraController._angleRightLimitMax;

  return *this;
}

// Move constructor
Controller5DoF::Controller5DoF(Controller5DoF &&cameraController) noexcept
    : Controller3DoF{dynamic_cast<Controller3DoF &&>(cameraController)},
      _angleUp{std::exchange(cameraController._angleUp, 0.0f)},
      _angleRight{std::exchange(cameraController._angleRight, 0.0f)},
      _angleUpLimitMin{std::exchange(cameraController._angleUpLimitMin, 0.0f)},
      _angleUpLimitMax{std::exchange(cameraController._angleUpLimitMax, 0.0f)},
      _angleRightLimitMin{std::exchange(cameraController._angleRightLimitMin, 0.0f)},
      _angleRightLimitMax{std::exchange(cameraController._angleRightLimitMax, 0.0f)} {}

// Move assignment operator
Controller5DoF &Controller5DoF::operator=(Controller5DoF &&cameraController) noexcept {
  this->Controller3DoF::operator=(dynamic_cast<Controller3DoF &&>(cameraController));

  std::swap(_angleUp, cameraController._angleUp);
  std::swap(_angleRight, cameraController._angleRight);
  std::swap(_angleUpLimitMin, cameraController._angleUpLimitMin);
  std::swap(_angleUpLimitMax, cameraController._angleUpLimitMax);
  std::swap(_angleRightLimitMin, cameraController._angleRightLimitMin);
  std::swap(_angleRightLimitMax, cameraController._angleRightLimitMax);

  return *this;
}

// Destructor
Controller5DoF::~Controller5DoF() noexcept {}

// Setters

void Controller5DoF::setCamera(BaseCamera *camera) noexcept {
  _camera = camera;
  updateLook();
}

// Other member functions

void Controller5DoF::updateLook() noexcept {
  glm::vec3 look                          = _camera->_forward;
  float     absoluteAngleBetweenLookAndUp = glm::angle(look, _camera->_worldUp);
  float     rotationAngle                 = absoluteAngleBetweenLookAndUp - glm::radians(90.0f);
  glm::vec3 rotationAxis{glm::cross(look, _camera->_worldUp)};
  _baseLookDirection = glm::rotate(look, rotationAngle, rotationAxis);

  setAngles(0.0f, -rotationAngle);
}

void Controller5DoF::moveUp(float distance) noexcept {
  _camera->_pos += _camera->_worldUp * distance;
}

void Controller5DoF::setAngles(float angleUp, float angleRight) noexcept {
  _angleUp    = angleUp;
  _angleUp    = angleIntoBounds(_angleUp, _angleUpLimitMin, _angleUpLimitMax);
  _angleRight = angleRight;
  _angleRight = angleIntoBounds(_angleRight, _angleRightLimitMin, _angleRightLimitMax);
}

void Controller5DoF::getAngles(float &angleUp, float &angleRight) const noexcept {
  angleUp    = _angleUp;
  angleRight = _angleRight;
}

void Controller5DoF::setAngleLimits(float angleUpLimitMin, float angleUpLimitMax,
                                    float angleRightLimitMin, float angleRightLimitMax) noexcept {
  _angleUpLimitMin    = angleUpLimitMin;
  _angleUpLimitMax    = angleUpLimitMax;
  _angleRightLimitMin = angleRightLimitMin;
  _angleRightLimitMax = angleRightLimitMax;
}

void Controller5DoF::getAngleLimits(float &angleUpLimitMin, float &angleUpLimitMax,
                                    float &angleRightLimitMin,
                                    float &angleRightLimitMax) const noexcept {
  angleUpLimitMin    = _angleUpLimitMin;
  angleUpLimitMax    = _angleUpLimitMax;
  angleRightLimitMin = _angleRightLimitMin;
  angleRightLimitMax = _angleRightLimitMax;
}

void Controller5DoF::addAngles(float angleUp, float angleRight) noexcept {
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
