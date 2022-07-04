#ifndef GLENGINE_SCENE_CAMERA_CAMERA6DOFCONTROLLER_HPP
#define GLENGINE_SCENE_CAMERA_CAMERA6DOFCONTROLLER_HPP

// "glengine" internal library
#include "./Camera3DoFController.hpp"

namespace glengine {

class Camera6DoFController : public Camera3DoFController {
 public:
  Camera6DoFController(BaseCamera *camera) noexcept;
  Camera6DoFController(const Camera3DoFController &cameraController) noexcept;
  Camera6DoFController &operator=(const Camera3DoFController &cameraController) noexcept;
  Camera6DoFController(const Camera6DoFController &cameraController) noexcept;
  Camera6DoFController &operator=(const Camera6DoFController &cameraController) noexcept;
  Camera6DoFController(Camera6DoFController &&cameraController) noexcept;
  Camera6DoFController &operator=(Camera6DoFController &&cameraController) noexcept;
  virtual ~Camera6DoFController() noexcept;

  void rotateForward(float angle);
  void rotateRight(float angle);
  void rotateUp(float angle);
};

}  // namespace glengine

#endif
