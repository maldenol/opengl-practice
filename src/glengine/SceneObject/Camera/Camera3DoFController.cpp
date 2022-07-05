// Header file
#include "./Camera3DoFController.hpp"

// STD
#include <utility>

using namespace glengine;

// Constructors, assignment operators and destructor

// Default constructor
Camera3DoFController::Camera3DoFController() noexcept {}

// Parameterized constructor
Camera3DoFController::Camera3DoFController(BaseCamera *camera) noexcept : _camera{camera} {}

// Copy constructor
Camera3DoFController::Camera3DoFController(const Camera3DoFController &cameraController) noexcept
    : _camera{cameraController._camera} {}

// Copy assignment operator
Camera3DoFController &Camera3DoFController::operator=(
    const Camera3DoFController &cameraController) noexcept {
  _camera = cameraController._camera;

  return *this;
}

// Move constructor
Camera3DoFController::Camera3DoFController(Camera3DoFController &&cameraController) noexcept
    : _camera{std::exchange(cameraController._camera, nullptr)} {}

// Move assignment operator
Camera3DoFController &Camera3DoFController::operator=(
    Camera3DoFController &&cameraController) noexcept {
  std::swap(_camera, cameraController._camera);

  return *this;
}

// Destructor
Camera3DoFController::~Camera3DoFController() noexcept {}

// Setters

void Camera3DoFController::setCamera(BaseCamera *camera) noexcept { _camera = camera; }

// Getters

const BaseCamera *Camera3DoFController::getCamera() const noexcept { return _camera; }

BaseCamera *Camera3DoFController::getCamera() noexcept { return _camera; }

// Other member functions

void Camera3DoFController::move(const glm::vec3 &deltaPos) noexcept { _camera->_pos += deltaPos; }

void Camera3DoFController::moveForward(float distance) noexcept {
  _camera->_pos += _camera->_forward * distance;
}

void Camera3DoFController::moveRight(float distance) noexcept {
  _camera->_pos += _camera->_right * distance;
}

void Camera3DoFController::moveUp(float distance) noexcept {
  _camera->_pos += _camera->_up * distance;
}
