#ifndef GLENGINE_SCENEOBJECT_COMPONENT_CAMERA_CONTROLLER_CONTROLLER5DOF_HPP
#define GLENGINE_SCENEOBJECT_COMPONENT_CAMERA_CONTROLLER_CONTROLLER5DOF_HPP

// "glengine" internal library
#include "./Controller3DoF.hpp"

namespace glengine {

class Controller5DoF : public Controller3DoF {
 private:
  float _angleUp{};
  float _angleRight{};
  float _angleUpLimitMin{};
  float _angleUpLimitMax{};
  float _angleRightLimitMin{};
  float _angleRightLimitMax{};

  glm::vec3 _baseLookDirection{};

 public:
  // Constructors, assignment operators and destructor
  Controller5DoF() noexcept;
  Controller5DoF(BaseCamera *camera) noexcept;
  Controller5DoF(const Controller3DoF &cameraController) noexcept;
  Controller5DoF &operator=(const Controller3DoF &cameraController) noexcept;
  Controller5DoF(const Controller5DoF &cameraController) noexcept;
  Controller5DoF &operator=(const Controller5DoF &cameraController) noexcept;
  Controller5DoF(Controller5DoF &&cameraController) noexcept;
  Controller5DoF &operator=(Controller5DoF &&cameraController) noexcept;
  virtual ~Controller5DoF() noexcept;

  // Setters
  void setCamera(BaseCamera *camera) noexcept override;

  // Other member functions
  void updateLook() noexcept;

  void moveUp(float distance) noexcept override;

  void setAngles(float angleUp, float angleRight) noexcept;
  void getAngles(float &angleUp, float &angleRight) const noexcept;

  void setAngleLimits(float angleUpLimitMin, float angleUpLimitMax, float angleRightLimitMin,
                      float angleRightLimitMax) noexcept;
  void getAngleLimits(float &angleUpLimitMin, float &angleUpLimitMax, float &angleRightLimitMin,
                      float &angleRightLimitMax) const noexcept;

  void addAngles(float angleUp, float angleRight) noexcept;
};

}  // namespace glengine

#endif
