#ifndef GLENGINE_SCENEOBJECT_CAMERA_ORTHOGRAPHICCAMERA_HPP
#define GLENGINE_SCENEOBJECT_CAMERA_ORTHOGRAPHICCAMERA_HPP

// "glengine" internal library
#include "./BaseCamera.hpp"

namespace glengine {

class OrthographicCamera : public BaseCamera {
 private:
  float _leftBorder{};
  float _rightBorder{};
  float _bottomBorder{};
  float _topBorder{};
  float _nearPlane{};
  float _farPlane{};

 public:
  // Constructors, assignment operators and destructor
  OrthographicCamera() noexcept;
  OrthographicCamera(const BaseCamera &camera) noexcept;
  OrthographicCamera &operator=(const BaseCamera &camera) noexcept;
  OrthographicCamera(const OrthographicCamera &camera) noexcept;
  OrthographicCamera &operator=(const OrthographicCamera &camera) noexcept;
  OrthographicCamera(OrthographicCamera &&camera) noexcept;
  OrthographicCamera &operator=(OrthographicCamera &&camera) noexcept;
  virtual ~OrthographicCamera() noexcept;

  // Other member functions
  void setProjectionAttributes(float leftBorder, float rightBorder, float bottomBorder,
                               float topBorder, float nearPlane, float farPlane) noexcept;
  void getProjectionAttributes(float &leftBorder, float &rightBorder, float &bottomBorder,
                               float &topBorder, float &nearPlane, float &farPlane) const noexcept;

  glm::mat4 getProjectionMatrix() const noexcept override;
};

}  // namespace glengine

#endif
