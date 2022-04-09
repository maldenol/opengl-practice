#ifndef CAMERA_PERSPECTIVECAMERA_HPP
#define CAMERA_PERSPECTIVECAMERA_HPP

// BaseCamera
#include "BaseCamera.hpp"

namespace glservice {

class PerspectiveCamera : public BaseCamera {
 protected:
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

  void setProjection(float verticalFOV, float aspectRatio, float nearPlane,
                     float farPlane);

  glm::mat4 getProjMatrix() const noexcept final;
};

}  // namespace glservice

#endif
