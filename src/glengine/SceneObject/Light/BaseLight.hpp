#ifndef GLENGINE_SCENE_LIGHT_BASELIGHT_HPP
#define GLENGINE_SCENE_LIGHT_BASELIGHT_HPP

// GLM
#include <glm/glm.hpp>

namespace glengine {

// Base light class
class BaseLight {
 private:
  glm::vec3 _color{};
  float     _intensity{};

 protected:
  BaseLight() noexcept;
  BaseLight(const glm::vec3 &color, float intensity) noexcept;
  BaseLight(const BaseLight &light) noexcept;
  BaseLight &operator=(const BaseLight &light) noexcept;
  BaseLight(BaseLight &&light) noexcept;
  BaseLight &operator=(BaseLight &&light) noexcept;

 public:
  virtual ~BaseLight() noexcept;

  void setColor(const glm::vec3 &color) noexcept;
  void setIntensity(float intensity) noexcept;

  const glm::vec3 &getColor() const noexcept;
  glm::vec3       &getColor() noexcept;
  float            getIntensity() const noexcept;
  float           &getIntensity() noexcept;
};

}  // namespace glengine

#endif
