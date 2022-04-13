// Header file
#include <glservice/camera/Camera3DoFController.hpp>

// STD
#include <utility>

glservice::Camera3DoFController::Camera3DoFController(BaseCamera *camera) noexcept
    : _camera{camera} {}

glservice::Camera3DoFController::Camera3DoFController(
    const Camera3DoFController &cameraController) noexcept
    : _camera{cameraController._camera} {}

glservice::Camera3DoFController &glservice::Camera3DoFController::operator=(
    const Camera3DoFController &cameraController) noexcept {
  _camera = cameraController._camera;

  return *this;
}

glservice::Camera3DoFController::Camera3DoFController(
    Camera3DoFController &&cameraController) noexcept
    : _camera{std::exchange(cameraController._camera, nullptr)} {}

glservice::Camera3DoFController &glservice::Camera3DoFController::operator=(
    Camera3DoFController &&cameraController) noexcept {
  std::swap(_camera, cameraController._camera);

  return *this;
}

glservice::Camera3DoFController::~Camera3DoFController() noexcept {}

void glservice::Camera3DoFController::setCamera(BaseCamera *camera) noexcept { _camera = camera; }

const glservice::BaseCamera *glservice::Camera3DoFController::getCamera() const noexcept {
  return _camera;
}

void glservice::Camera3DoFController::move(const glm::vec3 &deltaPos) noexcept {
  _camera->_pos += deltaPos;
}

void glservice::Camera3DoFController::moveForward(float distance) noexcept {
  _camera->_pos += _camera->_forward * distance;
}

void glservice::Camera3DoFController::moveRight(float distance) noexcept {
  _camera->_pos += _camera->_right * distance;
}

void glservice::Camera3DoFController::moveUp(float distance) noexcept {
  _camera->_pos += _camera->_up * distance;
}
