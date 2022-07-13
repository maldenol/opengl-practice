#ifndef GLENGINE_SCENEOBJECT_CAMERA_CONTROLLER_CONTROLLER3DOF_HPP
#define GLENGINE_SCENEOBJECT_CAMERA_CONTROLLER_CONTROLLER3DOF_HPP

// "glengine" internal library
#include "../BaseCamera.hpp"

namespace glengine {

class Controller3DoF {
 protected:
  BaseCamera *_camera{};

 public:
  // Constructors, assignment operators and destructor
  Controller3DoF() noexcept;
  Controller3DoF(BaseCamera *camera) noexcept;
  Controller3DoF(const Controller3DoF &cameraController) noexcept;
  Controller3DoF &operator=(const Controller3DoF &cameraController) noexcept;
  Controller3DoF(Controller3DoF &&cameraController) noexcept;
  Controller3DoF &operator=(Controller3DoF &&cameraController) noexcept;
  virtual ~Controller3DoF() noexcept;

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
