// Header file
#include "./SpotLight.hpp"

// STD
#include <utility>

using namespace glengine;

SpotLight::SpotLight() noexcept {}

SpotLight::SpotLight(const glm::vec3 &color, float intensity, const glm::vec3 &direction,
                     float linAttCoef, float quadAttCoef, float angle, float smoothAngle) noexcept
    : BaseLight{color, intensity},
      DirectionalLight{color, intensity, direction},
      PointLight{color, intensity, linAttCoef, quadAttCoef},
      _angle{angle},
      _smoothAngle{smoothAngle} {}

SpotLight::SpotLight(const BaseLight &light) noexcept : BaseLight{light} {}

SpotLight &SpotLight::operator=(const BaseLight &light) noexcept {
  this->BaseLight::operator=(light);

  return *this;
}

SpotLight::SpotLight(const SpotLight &light) noexcept
    : BaseLight{dynamic_cast<const BaseLight &>(light)},
      DirectionalLight{dynamic_cast<const DirectionalLight &>(light)},
      PointLight{dynamic_cast<const PointLight &>(light)},
      _angle{light._angle},
      _smoothAngle{light._smoothAngle} {}

SpotLight &SpotLight::operator=(const SpotLight &light) noexcept {
  this->BaseLight::       operator=(dynamic_cast<const BaseLight &>(light));
  this->DirectionalLight::operator=(dynamic_cast<const DirectionalLight &>(light));
  this->PointLight::      operator=(dynamic_cast<const PointLight &>(light));

  _angle       = light._angle;
  _smoothAngle = light._smoothAngle;

  return *this;
}

SpotLight::SpotLight(SpotLight &&light) noexcept
    : BaseLight{dynamic_cast<BaseLight &&>(light)},
      DirectionalLight{dynamic_cast<DirectionalLight &&>(light)},
      PointLight{dynamic_cast<PointLight &&>(light)},
      _angle{std::exchange(light._angle, 0.0f)},
      _smoothAngle{std::exchange(light._smoothAngle, 0.0f)} {}

SpotLight &SpotLight::operator=(SpotLight &&light) noexcept {
  this->BaseLight::       operator=(dynamic_cast<BaseLight &&>(light));
  this->DirectionalLight::operator=(dynamic_cast<DirectionalLight &&>(light));
  this->PointLight::      operator=(dynamic_cast<PointLight &&>(light));

  std::swap(_angle, light._angle);
  std::swap(_smoothAngle, light._smoothAngle);

  return *this;
}

void SpotLight::setAngle(float angle) noexcept { _angle = angle; }

void SpotLight::setSmoothAngle(float smoothAngle) noexcept { _smoothAngle = smoothAngle; }

float SpotLight::getAngle() const noexcept { return _angle; }

float &SpotLight::getAngle() noexcept { return _angle; }

float SpotLight::getSmoothAngle() const noexcept { return _smoothAngle; }

float &SpotLight::getSmoothAngle() noexcept { return _smoothAngle; }
