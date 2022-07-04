// Header file
#include "./DirectionalLight.hpp"

// STD
#include <utility>

using namespace glengine;

DirectionalLight::DirectionalLight() noexcept {}

DirectionalLight::DirectionalLight(const glm::vec3 &color, float intensity,
                                   const glm::vec3 &direction) noexcept
    : BaseLight{color, intensity}, _direction{direction} {}

DirectionalLight::DirectionalLight(const BaseLight &light) noexcept : BaseLight{light} {}

DirectionalLight &DirectionalLight::operator=(const BaseLight &light) noexcept {
  this->BaseLight::operator=(light);

  return *this;
}

DirectionalLight::DirectionalLight(const DirectionalLight &light) noexcept
    : BaseLight{dynamic_cast<const BaseLight &>(light)}, _direction{light._direction} {}

DirectionalLight &DirectionalLight::operator=(const DirectionalLight &light) noexcept {
  this->BaseLight::operator=(dynamic_cast<const BaseLight &>(light));

  _direction = light._direction;

  return *this;
}

DirectionalLight::DirectionalLight(DirectionalLight &&light) noexcept
    : BaseLight{dynamic_cast<BaseLight &&>(light)},
      _direction{std::exchange(light._direction, glm::vec3{})} {}

DirectionalLight &DirectionalLight::operator=(DirectionalLight &&light) noexcept {
  this->BaseLight::operator=(dynamic_cast<BaseLight &&>(light));

  std::swap(_direction, light._direction);

  return *this;
}

void DirectionalLight::setDirection(const glm::vec3 &direction) noexcept { _direction = direction; }

const glm::vec3 &DirectionalLight::getDirection() const noexcept { return _direction; }

glm::vec3 &DirectionalLight::getDirection() noexcept { return _direction; }
