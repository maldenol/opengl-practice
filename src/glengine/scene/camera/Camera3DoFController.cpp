// Header file
#include "./Camera3DoFController.hpp"

// STD
#include <utility>

glengine::Camera3DoFController::Camera3DoFController(BaseCamera *camera) noexcept
    : _camera{camera} {}

glengine::Camera3DoFController::Camera3DoFController(
    const Camera3DoFController &cameraController) noexcept
    : _camera{cameraController._camera} {}

glengine::Camera3DoFController &glengine::Camera3DoFController::operator=(
    const Camera3DoFController &cameraController) noexcept {
  _camera = cameraController._camera;

  return *this;
}

glengine::Camera3DoFController::Camera3DoFController(
    Camera3DoFController &&cameraController) noexcept
    : _camera{std::exchange(cameraController._camera, nullptr)} {}

glengine::Camera3DoFController &glengine::Camera3DoFController::operator=(
    Camera3DoFController &&cameraController) noexcept {
  std::swap(_camera, cameraController._camera);

  return *this;
}

glengine::Camera3DoFController::~Camera3DoFController() noexcept {}

void glengine::Camera3DoFController::setCamera(BaseCamera *camera) noexcept { _camera = camera; }

const glengine::BaseCamera *glengine::Camera3DoFController::getCamera() const noexcept {
  return _camera;
}

void glengine::Camera3DoFController::move(const glm::vec3 &deltaPos) noexcept {
  _camera->_pos += deltaPos;
}

void glengine::Camera3DoFController::moveForward(float distance) noexcept {
  _camera->_pos += _camera->_forward * distance;
}

void glengine::Camera3DoFController::moveRight(float distance) noexcept {
  _camera->_pos += _camera->_right * distance;
}

void glengine::Camera3DoFController::moveUp(float distance) noexcept {
  _camera->_pos += _camera->_up * distance;
}
