#ifndef GLENGINE_SCENEOBJECT_CAMERA_BASECAMERA_HPP
#define GLENGINE_SCENEOBJECT_CAMERA_BASECAMERA_HPP

// GLM
#include <glm/glm.hpp>

namespace glengine {

class BaseCamera {
 private:
  glm::vec3 _pos{};
  glm::vec3 _worldUp{};
  glm::vec3 _forward{};
  glm::vec3 _right{};
  glm::vec3 _up{};

 protected:
  // Constructors, assignment operators and destructor
  BaseCamera() noexcept;
  BaseCamera(const glm::vec3 &pos, const glm::vec3 &worldUp, const glm::vec3 &lookDir) noexcept;
  BaseCamera(const BaseCamera &camera) noexcept;
  BaseCamera &operator=(const BaseCamera &camera) noexcept;
  BaseCamera(BaseCamera &&camera) noexcept;
  BaseCamera &operator=(BaseCamera &&camera) noexcept;
  virtual ~BaseCamera() noexcept;

 public:
  // Setters
  void setPos(const glm::vec3 &pos) noexcept;
  void setWorldUp(const glm::vec3 &worldUp) noexcept;
  void setForward(const glm::vec3 &forward) noexcept;
  void setRight(const glm::vec3 &right) noexcept;
  void setUp(const glm::vec3 &up) noexcept;

  // Getters
  const glm::vec3 &getPos() const noexcept;
  glm::vec3       &getPos() noexcept;
  const glm::vec3 &getWorldUp() const noexcept;
  glm::vec3       &getWorldUp() noexcept;
  const glm::vec3 &getForward() const noexcept;
  glm::vec3       &getForward() noexcept;
  const glm::vec3 &getRight() const noexcept;
  glm::vec3       &getRight() noexcept;
  const glm::vec3 &getUp() const noexcept;
  glm::vec3       &getUp() noexcept;

  // Other member functions
  void look(const glm::vec3 &look) noexcept;
  void lookAt(const glm::vec3 &lookAt) noexcept;

  glm::mat4         getViewMatrix() const noexcept;
  virtual glm::mat4 getProjectionMatrix() const noexcept = 0;

  // Friend classes
  friend class Controller3DoF;
  friend class Controller5DoF;
  friend class Controller6DoF;
};

}  // namespace glengine

#endif
