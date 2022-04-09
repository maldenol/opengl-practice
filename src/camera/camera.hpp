#ifndef CAMERA_HPP
#define CAMERA_HPP

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Basic camera class
class BaseCamera {
protected:
  glm::vec3 _pos{};
  glm::vec3 _forward{};
  glm::vec3 _right{};
  glm::vec3 _up{};
  glm::vec3 _lookAt{};

  BaseCamera() noexcept;
  BaseCamera(const BaseCamera &camera) noexcept;
  BaseCamera &operator=(const BaseCamera &camera) noexcept;
  BaseCamera(BaseCamera &&camera) noexcept;
  BaseCamera &operator=(BaseCamera &&camera) noexcept;
  virtual ~BaseCamera() noexcept;

public:
  void setPosition(float eyeX, float eyeY, float eyeZ) noexcept;
  void lookAt(float centerX, float centerY, float centerZ) noexcept;

  const glm::vec3 &getPosition() const noexcept;
  const glm::vec3 &getForwardDirection() const noexcept;
  const glm::vec3 &getRightDirection() const noexcept;
  const glm::vec3 &getUpDirection() const noexcept;
  const glm::vec3 &getLookAtCenter() const noexcept;

  glm::mat4 getViewMatrix() const noexcept;
  virtual glm::mat4 getProjMatrix() const noexcept = 0;
};

class OrthographicCamera : public BaseCamera {
protected:
  float _leftBorder{};
  float _rightBorder{};
  float _bottomBorder{};
  float _topBorder{};
  float _nearPlane{};
  float _farPlane{};

public:
  OrthographicCamera() noexcept;
  OrthographicCamera(const BaseCamera &camera) noexcept;
  OrthographicCamera &operator=(const BaseCamera &camera) noexcept;
  OrthographicCamera(const OrthographicCamera &camera) noexcept;
  OrthographicCamera &operator=(const OrthographicCamera &camera) noexcept;
  OrthographicCamera(OrthographicCamera &&camera) noexcept;
  OrthographicCamera &operator=(OrthographicCamera &&camera) noexcept;
  virtual ~OrthographicCamera() noexcept;

  void setProjection(float leftBorder, float rightBorder, float bottomBorder, float topBorder, float nearPlane, float farPlane);

  virtual glm::mat4 getProjMatrix() const noexcept override final;
};

class PerspectiveCamera : public BaseCamera {
protected:
  float _verticalFOV{};
  float _aspectRatio{};
  float _nearPlane{};
  float _farPlane{};

public:
  PerspectiveCamera() noexcept;
  PerspectiveCamera(const BaseCamera &camera) noexcept;
  PerspectiveCamera &operator=(const BaseCamera &camera) noexcept;
  PerspectiveCamera(const PerspectiveCamera &camera) noexcept;
  PerspectiveCamera &operator=(const PerspectiveCamera &camera) noexcept;
  PerspectiveCamera(PerspectiveCamera &&camera) noexcept;
  PerspectiveCamera &operator=(PerspectiveCamera &&camera) noexcept;
  virtual ~PerspectiveCamera() noexcept;

  void setProjection(float verticalFOV, float aspectRatio, float nearPlane, float farPlane);

  virtual glm::mat4 getProjMatrix() const noexcept override final;
};

#endif
