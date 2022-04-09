#ifndef CAMERA_PERSPECTIVECAMERA_HPP
#define CAMERA_PERSPECTIVECAMERA_HPP

// BaseCamera
#include "BaseCamera.hpp"

namespace glservice {

class PerspectiveCamera : public BaseCamera {
 private:
  float _verticalFOV{};
  float _aspectRatio{};
  float _nearPlane{};
  float _farPlane{};

 public:
  PerspectiveCamera() noexcept;
  PerspectiveCamera(const BaseCamera &camera) noexcept;
  PerspectiveCamera &operator=(const BaseCamera &camera) noexcept;
  PerspectiveCamera(const PerspectiveCamera &camera) noexcept;
  PerspectiveCamera &operator=(const PerspectiveCamera &camera) noexcept;
  PerspectiveCamera(PerspectiveCamera &&camera) noexcept;
  PerspectiveCamera &operator=(PerspectiveCamera &&camera) noexcept;
  virtual ~PerspectiveCamera() noexcept;

  void setProjectionAttributes(float verticalFOV, float aspectRatio,
                               float nearPlane, float farPlane) noexcept;

  void getProjectionAttributes(float &verticalFOV, float &aspectRatio,
                               float &nearPlane,
                               float &farPlane) const noexcept;

  glm::mat4 getProjectionMatrix() const noexcept final;
};

}  // namespace glservice

#endif
