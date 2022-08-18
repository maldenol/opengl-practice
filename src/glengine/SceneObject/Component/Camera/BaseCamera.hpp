#ifndef GLENGINE_SCENEOBJECT_COMPONENT_CAMERA_BASECAMERA_HPP
#define GLENGINE_SCENEOBJECT_COMPONENT_CAMERA_BASECAMERA_HPP

// GLM
#include <glm/glm.hpp>

// "glengine" internal library
#include "../Component.hpp"

namespace glengine {

class BaseCamera : public Component {
 private:
  glm::vec3 _position{};
  glm::vec3 _worldUp{};
  glm::vec3 _forward{};
  glm::vec3 _right{};
  glm::vec3 _up{};

  glm::mat4 _viewMatrix{};

 protected:
  glm::mat4 _projectionMatrix{};

 protected:
  // Constructors, assignment operators and destructor
  BaseCamera() noexcept;
  BaseCamera(const glm::vec3 &position, const glm::vec3 &worldUp,
             const glm::vec3 &lookDir) noexcept;
  BaseCamera(const BaseCamera &camera) noexcept;
  BaseCamera &operator=(const BaseCamera &camera) noexcept;
  BaseCamera(BaseCamera &&camera) noexcept;
  BaseCamera &operator=(BaseCamera &&camera) noexcept;
  virtual ~BaseCamera() noexcept;

 public:
  // Setters
  void setPosition(const glm::vec3 &position) noexcept;
  void setWorldUp(const glm::vec3 &worldUp) noexcept;
  void setForward(const glm::vec3 &forward) noexcept;
  void setRight(const glm::vec3 &right) noexcept;
  void setUp(const glm::vec3 &up) noexcept;
  void setViewMatrix(const glm::mat4 &viewMatrix) noexcept;
  void setProjectionMatrix(const glm::mat4 &projectionMatrix) noexcept;

  // Getters
  const glm::vec3 &getPosition() const noexcept;
  glm::vec3       &getPosition() noexcept;
  const glm::vec3 &getWorldUp() const noexcept;
  glm::vec3       &getWorldUp() noexcept;
  const glm::vec3 &getForward() const noexcept;
  glm::vec3       &getForward() noexcept;
  const glm::vec3 &getRight() const noexcept;
  glm::vec3       &getRight() noexcept;
  const glm::vec3 &getUp() const noexcept;
  glm::vec3       &getUp() noexcept;
  const glm::mat4 &getViewMatrix() const noexcept;
  glm::mat4       &getViewMatrix() noexcept;
  const glm::mat4 &getProjectionMatrix() const noexcept;
  glm::mat4       &getProjectionMatrix() noexcept;

  // Other member functions
  void         recalculateViewMatrix() noexcept;
  virtual void recalculateProjectionMatrix() noexcept = 0;

  void look(const glm::vec3 &look) noexcept;
  void lookAt(const glm::vec3 &lookAt) noexcept;
};

}  // namespace glengine

#endif
