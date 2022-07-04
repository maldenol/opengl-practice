#ifndef GLENGINE_SCENE_CAMERA_CAMERA5DOFCONTROLLER_HPP
#define GLENGINE_SCENE_CAMERA_CAMERA5DOFCONTROLLER_HPP

// "glengine" internal library
#include "./Camera3DoFController.hpp"

namespace glengine {

class Camera5DoFController : public Camera3DoFController {
 private:
  float _angleUp{};
  float _angleRight{};
  float _angleUpLimitMin{};
  float _angleUpLimitMax{};
  float _angleRightLimitMin{};
  float _angleRightLimitMax{};

  glm::vec3 _baseLookDirection{};

 public:
  Camera5DoFController(BaseCamera *camera) noexcept;
  Camera5DoFController(const Camera3DoFController &cameraController) noexcept;
  Camera5DoFController &operator=(const Camera3DoFController &cameraController) noexcept;
  Camera5DoFController(const Camera5DoFController &cameraController) noexcept;
  Camera5DoFController &operator=(const Camera5DoFController &cameraController) noexcept;
  Camera5DoFController(Camera5DoFController &&cameraController) noexcept;
  Camera5DoFController &operator=(Camera5DoFController &&cameraController) noexcept;
  virtual ~Camera5DoFController() noexcept;

  void setCamera(BaseCamera *camera) noexcept override;
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
