#ifndef GLENGINE_SCENEOBJECT_CAMERA_CAMERA3DOFCONTROLLER_HPP
#define GLENGINE_SCENEOBJECT_CAMERA_CAMERA3DOFCONTROLLER_HPP

// "glengine" internal library
#include "./BaseCamera.hpp"

namespace glengine {

class Camera3DoFController {
 protected:
  BaseCamera *_camera{};

 public:
  // Constructors, assignment operators and destructor
  Camera3DoFController() noexcept;
  Camera3DoFController(BaseCamera *camera) noexcept;
  Camera3DoFController(const Camera3DoFController &cameraController) noexcept;
  Camera3DoFController &operator=(const Camera3DoFController &cameraController) noexcept;
  Camera3DoFController(Camera3DoFController &&cameraController) noexcept;
  Camera3DoFController &operator=(Camera3DoFController &&cameraController) noexcept;
  virtual ~Camera3DoFController() noexcept;

  // Setters
  virtual void setCamera(BaseCamera *camera) noexcept;

  // Getters
  const BaseCamera *getCamera() const noexcept;
  BaseCamera       *getCamera() noexcept;

  // Other member functions
  void         move(const glm::vec3 &deltaPos) noexcept;
  void         moveForward(float distance) noexcept;
  void         moveRight(float distance) noexcept;
  virtual void moveUp(float distance) noexcept;
};

}  // namespace glengine

#endif
