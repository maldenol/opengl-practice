#ifndef GLENGINE_SCENEOBJECT_COMPONENT_CAMERA_CONTROLLER_CONTROLLER6DOF_HPP
#define GLENGINE_SCENEOBJECT_COMPONENT_CAMERA_CONTROLLER_CONTROLLER6DOF_HPP

// "glengine" internal library
#include "./Controller3DoF.hpp"

namespace glengine {

class Controller6DoF : public Controller3DoF {
 public:
  // Constructors, assignment operators and destructor
  Controller6DoF() noexcept;
  Controller6DoF(BaseCamera *camera) noexcept;
  Controller6DoF(const Controller3DoF &cameraController) noexcept;
  Controller6DoF &operator=(const Controller3DoF &cameraController) noexcept;
  Controller6DoF(const Controller6DoF &cameraController) noexcept;
  Controller6DoF &operator=(const Controller6DoF &cameraController) noexcept;
  Controller6DoF(Controller6DoF &&cameraController) noexcept;
  Controller6DoF &operator=(Controller6DoF &&cameraController) noexcept;
  virtual ~Controller6DoF() noexcept;

  // Other member functions
  void rotateForward(float angle);
  void rotateRight(float angle);
  void rotateUp(float angle);
};

}  // namespace glengine

#endif
