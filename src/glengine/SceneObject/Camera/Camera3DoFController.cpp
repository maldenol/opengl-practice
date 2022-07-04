// Header file
#include "./Camera3DoFController.hpp"

// STD
#include <utility>

using namespace glengine;

Camera3DoFController::Camera3DoFController(BaseCamera *camera) noexcept : _camera{camera} {}

Camera3DoFController::Camera3DoFController(const Camera3DoFController &cameraController) noexcept
    : _camera{cameraController._camera} {}

Camera3DoFController &Camera3DoFController::operator=(
    const Camera3DoFController &cameraController) noexcept {
  _camera = cameraController._camera;

  return *this;
}

Camera3DoFController::Camera3DoFController(Camera3DoFController &&cameraController) noexcept
    : _camera{std::exchange(cameraController._camera, nullptr)} {}

Camera3DoFController &Camera3DoFController::operator=(
    Camera3DoFController &&cameraController) noexcept {
  std::swap(_camera, cameraController._camera);

  return *this;
}

Camera3DoFController::~Camera3DoFController() noexcept {}

void Camera3DoFController::setCamera(BaseCamera *camera) noexcept { _camera = camera; }

const BaseCamera *Camera3DoFController::getCamera() const noexcept { return _camera; }

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
