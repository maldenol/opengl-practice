#ifndef GLENGINE_SCENEOBJECT_LIGHT_SPOTLIGHT_HPP
#define GLENGINE_SCENEOBJECT_LIGHT_SPOTLIGHT_HPP

// "glengine" internal library
#include "./DirectionalLight.hpp"
#include "./PointLight.hpp"

namespace glengine {

// Spot light class
class SpotLight : public DirectionalLight, public PointLight {
 private:
  float _angle{};
  float _smoothAngle{};

 public:
  // Constructors, assignment operators and destructor
  SpotLight() noexcept;
  SpotLight(const glm::vec3 &color, float intensity, const glm::vec3 &direction, float linAttCoef,
            float quadAttCoef, float angle, float smoothAngle) noexcept;
  SpotLight(const BaseLight &light) noexcept;
  SpotLight &operator=(const BaseLight &light) noexcept;
  SpotLight(const DirectionalLight &light) noexcept;
  SpotLight &operator=(const DirectionalLight &light) noexcept;
  SpotLight(const PointLight &light) noexcept;
  SpotLight &operator=(const PointLight &light) noexcept;
  SpotLight(const SpotLight &light) noexcept;
  SpotLight &operator=(const SpotLight &light) noexcept;
  SpotLight(SpotLight &&light) noexcept;
  SpotLight &operator=(SpotLight &&light) noexcept;
  virtual ~SpotLight() noexcept;

  // Setters
  void setAngle(float angle) noexcept;
  void setSmoothAngle(float smoothAngle) noexcept;
  void setShadowMapTextureResolution(GLsizei shadowMapTextureResolution) noexcept override;

  // Getters
  float  getAngle() const noexcept;
  float &getAngle() noexcept;
  float  getSmoothAngle() const noexcept;
  float &getSmoothAngle() noexcept;
};

}  // namespace glengine

#endif
