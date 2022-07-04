// Header file
#include "./BaseLight.hpp"

// STD
#include <utility>

using namespace glengine;

BaseLight::BaseLight() noexcept {}

BaseLight::BaseLight(const glm::vec3 &color, float intensity) noexcept
    : _color{color}, _intensity{intensity} {}

BaseLight::BaseLight(const BaseLight &light) noexcept
    : _color{light._color}, _intensity{light._intensity} {}

BaseLight &BaseLight::operator=(const BaseLight &light) noexcept {
  _color     = light._color;
  _intensity = light._intensity;

  return *this;
}

BaseLight::BaseLight(BaseLight &&light) noexcept
    : _color{std::exchange(light._color, glm::vec3{})},
      _intensity{std::exchange(light._intensity, 0.0f)} {}

BaseLight &BaseLight::operator=(BaseLight &&light) noexcept {
  std::swap(_color, light._color);
  std::swap(_intensity, light._intensity);

  return *this;
}

BaseLight::~BaseLight() noexcept {}

void BaseLight::setColor(const glm::vec3 &color) noexcept { _color = color; }

void BaseLight::setIntensity(float intensity) noexcept { _intensity = intensity; }

const glm::vec3 &BaseLight::getColor() const noexcept { return _color; }

glm::vec3 &BaseLight::getColor() noexcept { return _color; }

float BaseLight::getIntensity() const noexcept { return _intensity; }

float &BaseLight::getIntensity() noexcept { return _intensity; }
