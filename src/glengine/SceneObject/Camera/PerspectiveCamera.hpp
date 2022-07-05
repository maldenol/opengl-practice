#ifndef GLENGINE_SCENEOBJECT_CAMERA_PERSPECTIVECAMERA_HPP
#define GLENGINE_SCENEOBJECT_CAMERA_PERSPECTIVECAMERA_HPP

// "glengine" internal library
#include "./BaseCamera.hpp"

namespace glengine {

class PerspectiveCamera : public BaseCamera {
 private:
  float _verticalFOV{};
  float _aspectRatio{};
  float _nearPlane{};
  float _farPlane{};

 public:
  // Constructors, assignment operators and destructor
  PerspectiveCamera() noexcept;
  PerspectiveCamera(const BaseCamera &camera) noexcept;
  PerspectiveCamera &operator=(const BaseCamera &camera) noexcept;
  PerspectiveCamera(const PerspectiveCamera &camera) noexcept;
  PerspectiveCamera &operator=(const PerspectiveCamera &camera) noexcept;
  PerspectiveCamera(PerspectiveCamera &&camera) noexcept;
  PerspectiveCamera &operator=(PerspectiveCamera &&camera) noexcept;
  virtual ~PerspectiveCamera() noexcept;

  // Other member functions
  void setProjectionAttributes(float verticalFOV, float aspectRatio, float nearPlane,
                               float farPlane) noexcept;
  void getProjectionAttributes(float &verticalFOV, float &aspectRatio, float &nearPlane,
                               float &farPlane) const noexcept;

  glm::mat4 getProjectionMatrix() const noexcept override;
};

}  // namespace glengine

#endif
