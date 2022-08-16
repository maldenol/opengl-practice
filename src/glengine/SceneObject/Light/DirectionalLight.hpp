#ifndef GLENGINE_SCENEOBJECT_LIGHT_DIRECTIONALLIGHT_HPP
#define GLENGINE_SCENEOBJECT_LIGHT_DIRECTIONALLIGHT_HPP

// "glengine" internal library
#include "./BaseLight.hpp"

namespace glengine {

// Directional light class
class DirectionalLight : virtual public BaseLight {
 private:
  glm::vec3 _direction{};

 public:
  // Constructors, assignment operators and destructor
  DirectionalLight() noexcept;
  DirectionalLight(const glm::vec3 &color, const glm::vec3 &direction) noexcept;
  DirectionalLight(const BaseLight &light) noexcept;
  DirectionalLight &operator=(const BaseLight &light) noexcept;
  DirectionalLight(const DirectionalLight &light) noexcept;
  DirectionalLight &operator=(const DirectionalLight &light) noexcept;
  DirectionalLight(DirectionalLight &&light) noexcept;
  DirectionalLight &operator=(DirectionalLight &&light) noexcept;
  virtual ~DirectionalLight() noexcept;

  // Setters
  void setDirection(const glm::vec3 &direction) noexcept;
  void setShadowMapTextureResolution(GLsizei shadowMapTextureResolution) noexcept override;

  // Getters
  const glm::vec3 &getDirection() const noexcept;
  glm::vec3       &getDirection() noexcept;
};

}  // namespace glengine

#endif
