#ifndef GLENGINE_SCENE_LIGHT_POINTLIGHT_HPP
#define GLENGINE_SCENE_LIGHT_POINTLIGHT_HPP

// "glengine" internal library
#include "./BaseLight.hpp"

namespace glengine {

// Point light class
class PointLight : virtual public BaseLight {
 private:
  float _linAttCoef{};
  float _quadAttCoef{};

 public:
  PointLight() noexcept;
  PointLight(const glm::vec3 &color, float intensity, float linAttCoef, float quadAttCoef) noexcept;
  PointLight(const BaseLight &light) noexcept;
  PointLight &operator=(const BaseLight &light) noexcept;
  PointLight(const PointLight &light) noexcept;
  PointLight &operator=(const PointLight &light) noexcept;
  PointLight(PointLight &&light) noexcept;
  PointLight &operator=(PointLight &&light) noexcept;

  void setLinAttCoef(float linAttCoef) noexcept;
  void setQuadAttCoef(float quadAttCoef) noexcept;

  float  getLinAttCoef() const noexcept;
  float &getLinAttCoef() noexcept;
  float  getQuadAttCoef() const noexcept;
  float &getQuadAttCoef() noexcept;
};

}  // namespace glengine

#endif
