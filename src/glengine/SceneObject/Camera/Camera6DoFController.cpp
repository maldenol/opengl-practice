// Header file
#include "./Camera6DoFController.hpp"

// STD
#include <utility>

// GLM
#include <glm/gtx/rotate_vector.hpp>

using namespace glengine;

Camera6DoFController::Camera6DoFController(BaseCamera *camera) noexcept
    : Camera3DoFController{camera} {}

Camera6DoFController::Camera6DoFController(const Camera3DoFController &cameraController) noexcept
    : Camera3DoFController{cameraController} {}

Camera6DoFController &Camera6DoFController::operator=(
    const Camera3DoFController &cameraController) noexcept {
  this->Camera3DoFController::operator=(cameraController);

  return *this;
}

Camera6DoFController::Camera6DoFController(const Camera6DoFController &cameraController) noexcept
    : Camera3DoFController{dynamic_cast<const Camera3DoFController &>(cameraController)} {}

Camera6DoFController &Camera6DoFController::operator=(
    const Camera6DoFController &cameraController) noexcept {
  this->Camera3DoFController::operator=(
      dynamic_cast<const Camera3DoFController &>(cameraController));

  return *this;
}

Camera6DoFController::Camera6DoFController(Camera6DoFController &&cameraController) noexcept
    : Camera3DoFController{dynamic_cast<Camera3DoFController &&>(cameraController)} {}

Camera6DoFController &Camera6DoFController::operator=(
    Camera6DoFController &&cameraController) noexcept {
  this->Camera3DoFController::operator=(dynamic_cast<Camera3DoFController &&>(cameraController));

  return *this;
}

Camera6DoFController::~Camera6DoFController() noexcept {}

void Camera6DoFController::rotateForward(float angle) {
  // Precision dependent method
  _camera->_right = glm::rotate(_camera->_right, angle, _camera->_forward);
  _camera->_up    = glm::rotate(_camera->_up, angle, _camera->_forward);
}

void Camera6DoFController::rotateRight(float angle) {
  // Precision dependent method
  _camera->_up      = glm::rotate(_camera->_up, angle, _camera->_right);
  _camera->_forward = glm::rotate(_camera->_forward, angle, _camera->_right);
}

void Camera6DoFController::rotateUp(float angle) {
  // Precision dependent method
  _camera->_forward = glm::rotate(_camera->_forward, angle, _camera->_up);
  _camera->_right   = glm::rotate(_camera->_right, angle, _camera->_up);
}
