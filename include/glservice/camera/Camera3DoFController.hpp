#ifndef CAMERA_CAMERA3DOFCONTROLLER_HPP
#define CAMERA_CAMERA3DOFCONTROLLER_HPP

// BaseCamera
#include "BaseCamera.hpp"

namespace glservice {

class Camera3DoFController {
 protected:
  BaseCamera *_camera{};

 public:
  Camera3DoFController(BaseCamera *camera) noexcept;
  Camera3DoFController(const Camera3DoFController &cameraController) noexcept;
  Camera3DoFController &operator=(
      const Camera3DoFController &cameraController) noexcept;
  Camera3DoFController(Camera3DoFController &&cameraController) noexcept;
  Camera3DoFController &operator=(
      Camera3DoFController &&cameraController) noexcept;
  virtual ~Camera3DoFController() noexcept;

  virtual void      setCamera(BaseCamera *camera) noexcept;
  const BaseCamera *getCamera() const noexcept;

  void         move(const glm::vec3 &deltaPos) noexcept;
  void         moveForward(float distance) noexcept;
  void         moveRight(float distance) noexcept;
  virtual void moveUp(float distance) noexcept;
};

}  // namespace glservice

#endif
