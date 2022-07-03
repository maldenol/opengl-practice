#ifndef GLENGINE_SCENE_LIGHT_LIGHT_HPP
#define GLENGINE_SCENE_LIGHT_LIGHT_HPP

// GLM
#include <glm/glm.hpp>

namespace glengine {

// Base light struct
struct BaseLight {
  glm::vec3 color{};
  float     intensity{};

 protected:
  BaseLight(const glm::vec3 &color, float intensity) noexcept;

 public:
  virtual ~BaseLight() noexcept;
};

// Directional light struct
struct DirectionalLight : public BaseLight {
  glm::vec3 direction{};

  DirectionalLight(const glm::vec3 &color, float intensity, const glm::vec3 &direction) noexcept;
};

// Point light struct
struct PointLight : public BaseLight {
  float linAttCoef{};
  float quadAttCoef{};

  PointLight(const glm::vec3 &color, float intensity, float linAttCoef, float quadAttCoef) noexcept;
};

// Spot light struct
struct SpotLight : public BaseLight {
  glm::vec3 direction{};

  float linAttCoef{};
  float quadAttCoef{};

  float angle{};
  float smoothAngle{};

  SpotLight(const glm::vec3 &color, float intensity, const glm::vec3 &direction, float linAttCoef,
            float quadAttCoef, float angle, float smoothAngle) noexcept;
};

}  // namespace glengine

#endif
