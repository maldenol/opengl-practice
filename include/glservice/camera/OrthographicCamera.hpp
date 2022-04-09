#ifndef CAMERA_ORTHOGRAPHICCAMERA_HPP
#define CAMERA_ORTHOGRAPHICCAMERA_HPP

// BaseCamera
#include "BaseCamera.hpp"

namespace glservice {

class OrthographicCamera : public BaseCamera {
 protected:
  float _leftBorder{};
  float _rightBorder{};
  float _bottomBorder{};
  float _topBorder{};
  float _nearPlane{};
  float _farPlane{};

 public:
  OrthographicCamera() noexcept;
  OrthographicCamera(const BaseCamera &camera) noexcept;
  OrthographicCamera &operator=(const BaseCamera &camera) noexcept;
  OrthographicCamera(const OrthographicCamera &camera) noexcept;
  OrthographicCamera &operator=(const OrthographicCamera &camera) noexcept;
  OrthographicCamera(OrthographicCamera &&camera) noexcept;
  OrthographicCamera &operator=(OrthographicCamera &&camera) noexcept;
  virtual ~OrthographicCamera() noexcept;

  void setProjection(float leftBorder, float rightBorder, float bottomBorder,
                     float topBorder, float nearPlane, float farPlane);

  glm::mat4 getProjMatrix() const noexcept final;
};

}  // namespace glservice

#endif
