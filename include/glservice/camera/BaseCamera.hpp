#ifndef CAMERA_BASECAMERA_HPP
#define CAMERA_BASECAMERA_HPP

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace glservice {

class BaseCamera {
 private:
  glm::vec3 _pos{};
  glm::vec3 _forward{};
  glm::vec3 _right{};
  glm::vec3 _up{};
  glm::vec3 _lookAt{};

 protected:
  BaseCamera() noexcept;
  BaseCamera(const BaseCamera &camera) noexcept;
  BaseCamera &operator=(const BaseCamera &camera) noexcept;
  BaseCamera(BaseCamera &&camera) noexcept;
  BaseCamera &operator=(BaseCamera &&camera) noexcept;
  virtual ~BaseCamera() noexcept;

 public:
  void setPosition(const glm::vec3 &pos) noexcept;
  void lookAt(const glm::vec3 &lookAt) noexcept;

  const glm::vec3 &getPosition() const noexcept;
  const glm::vec3 &getForwardDirection() const noexcept;
  const glm::vec3 &getRightDirection() const noexcept;
  const glm::vec3 &getUpDirection() const noexcept;
  const glm::vec3 &getLookAtCenter() const noexcept;

  glm::mat4         getViewMatrix() const noexcept;
  virtual glm::mat4 getProjectionMatrix() const noexcept = 0;
};

}  // namespace glservice

#endif
