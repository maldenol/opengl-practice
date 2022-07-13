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

  // Setters
  void setVerticalFOV(float verticalVOF) noexcept;
  void setAspectRatio(float aspectRatio) noexcept;
  void setNearPlane(float nearPlane) noexcept;
  void setFarPlane(float farPlane) noexcept;

  // Getters
  float  getVerticalFOV() const noexcept;
  float &getVerticalFOV() noexcept;
  float  getAspectRatio() const noexcept;
  float &getAspectRatio() noexcept;
  float  getNearPlane() const noexcept;
  float &getNearPlane() noexcept;
  float  getFarPlane() const noexcept;
  float &getFarPlane() noexcept;

  // Other member functions
  glm::mat4 getProjectionMatrix() const noexcept override;
};

}  // namespace glengine

#endif
