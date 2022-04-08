// Header file
#include "camera.hpp"

// STD
#include <utility>

Camera::Camera() noexcept : _pos{glm::vec3{0.0f}} {
  this->lookAt(0.0f, 0.0f, -1.0f);
}

Camera::Camera(const Camera &camera) noexcept
    : _pos{camera._pos},
      _forward{camera._forward},
      _right{camera._right},
      _up{camera._up} {}

Camera &Camera::operator=(const Camera &camera) noexcept {
  *this = Camera{camera};
  return *this;
}

Camera::Camera(Camera &&camera) noexcept
    : _pos{std::exchange(camera._pos, glm::vec3{})},
      _forward{std::exchange(camera._forward, glm::vec3{})},
      _right{std::exchange(camera._right, glm::vec3{})},
      _up{std::exchange(camera._up, glm::vec3{})} {}

Camera &Camera::operator=(Camera &&camera) noexcept {
  std::swap(_pos, camera._pos);
  std::swap(_forward, camera._forward);
  std::swap(_right, camera._right);
  std::swap(_up, camera._up);
  std::swap(_lookAt, camera._lookAt);
  std::swap(_orthoNearPlane, camera._orthoNearPlane);
  std::swap(_orthoFarPlane, camera._orthoFarPlane);
  std::swap(_orthoLeftBorder, camera._orthoLeftBorder);
  std::swap(_orthoRightBorder, camera._orthoRightBorder);
  std::swap(_orthoBottomBorder, camera._orthoBottomBorder);
  std::swap(_orthoTopBorder, camera._orthoTopBorder);
  std::swap(_perspNearPlane, camera._perspNearPlane);
  std::swap(_perspFarPlane, camera._perspFarPlane);
  std::swap(_perspVerticalFOV, camera._perspVerticalFOV);
  std::swap(_perspAspectRatio, camera._perspAspectRatio);

  return *this;
}

Camera::~Camera() noexcept {}

void Camera::setPosition(float eyeX, float eyeY, float eyeZ) noexcept {
  _pos = glm::vec3{eyeX, eyeY, eyeZ};
}

void Camera::lookAt(float centerX, float centerY, float centerZ) noexcept {
  _lookAt  = glm::vec3{centerX, centerY, centerZ};
  _forward = glm::normalize(_lookAt - _pos);
  _right   = glm::cross(_forward, glm::vec3{0.0f, 1.0f, 0.0f});
  _up      = glm::cross(_right, _forward);
}

void Camera::setOrthographicProjection(float leftBorder, float rightBorder,
                                       float bottomBorder, float topBorder,
                                       float nearPlane, float farPlane) {
  _orthoLeftBorder   = leftBorder;
  _orthoRightBorder  = rightBorder;
  _orthoBottomBorder = bottomBorder;
  _orthoTopBorder    = topBorder;
  _orthoNearPlane    = nearPlane;
  _orthoFarPlane     = farPlane;
}

void Camera::setPerspectiveProjection(float verticalFOV, float aspectRatio,
                                      float nearPlane, float farPlane) {
  _perspVerticalFOV = verticalFOV;
  _perspAspectRatio = aspectRatio;
  _perspNearPlane   = nearPlane;
  _perspFarPlane    = farPlane;
}

glm::mat4 Camera::getViewMatrix() const noexcept {
  return glm::lookAt(_pos, _lookAt, _up);
}

glm::mat4 Camera::getOrthoProjMatrix() const noexcept {
  return glm::ortho(_orthoLeftBorder, _orthoRightBorder, _orthoBottomBorder,
                    _orthoTopBorder, _orthoNearPlane, _orthoFarPlane);
}

glm::mat4 Camera::getPerspProjMatrix() const noexcept {
  return glm::perspective(_perspVerticalFOV, _perspAspectRatio, _perspNearPlane,
                          _perspFarPlane);
}
