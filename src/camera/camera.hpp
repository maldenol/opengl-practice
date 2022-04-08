#ifndef CAMERA_HPP
#define CAMERA_HPP

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
private:
  glm::vec3 _pos{};
  glm::vec3 _forward{};
  glm::vec3 _right{};
  glm::vec3 _up{};
  glm::vec3 _lookAt{};

  float _orthoLeftBorder{-1.0f};
  float _orthoRightBorder{1.0f};
  float _orthoBottomBorder{-1.0f};
  float _orthoTopBorder{1.0f};
  float _orthoNearPlane{0.1f};
  float _orthoFarPlane{100.0f};

  float _perspVerticalFOV{glm::radians(45.0f)};
  float _perspAspectRatio{4.0f / 3.0f};
  float _perspNearPlane{0.1f};
  float _perspFarPlane{100.0f};

public:
  Camera() noexcept;
  Camera(const Camera &camera) noexcept;
  Camera &operator=(const Camera &camera) noexcept;
  Camera(Camera &&camera) noexcept;
  Camera &operator=(Camera &&camera) noexcept;
  ~Camera() noexcept;

  void setPosition(float eyeX, float eyeY, float eyeZ) noexcept;
  void lookAt(float centerX, float centerY, float centerZ) noexcept;
  void setOrthographicProjection(float leftBorder, float rightBorder, float bottomBorder, float topBorder, float nearPlane, float farPlane);
  void setPerspectiveProjection(float verticalFOV, float aspectRatio, float nearPlane, float farPlane);

  glm::mat4 getViewMatrix() const noexcept;
  glm::mat4 getOrthoProjMatrix() const noexcept;
  glm::mat4 getPerspProjMatrix() const noexcept;
};

#endif
