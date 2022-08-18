#ifndef GLENGINE_SCENEOBJECT_COMPONENT_CAMERA_ORTHOGRAPHICCAMERA_HPP
#define GLENGINE_SCENEOBJECT_COMPONENT_CAMERA_ORTHOGRAPHICCAMERA_HPP

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

  // Setters
  void setLeftBorder(float leftBorder) noexcept;
  void setRightBorder(float rightBorder) noexcept;
  void setBottomBorder(float bottomBorder) noexcept;
  void setTopBorder(float topBorder) noexcept;
  void setNearPlane(float nearPlane) noexcept;
  void setFarPlane(float farPlane) noexcept;

  // Getters
  float  getLeftBorder() const noexcept;
  float &getLeftBorder() noexcept;
  float  getRightBorder() const noexcept;
  float &getRightBorder() noexcept;
  float  getBottomBorder() const noexcept;
  float &getBottomBorder() noexcept;
  float  getTopBorder() const noexcept;
  float &getTopBorder() noexcept;
  float  getNearPlane() const noexcept;
  float &getNearPlane() noexcept;
  float  getFarPlane() const noexcept;
  float &getFarPlane() noexcept;

  // Other member functions
  void recalculateProjectionMatrix() noexcept override;
};

}  // namespace glengine

#endif
