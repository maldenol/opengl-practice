// Header file
#include "./DirectionalLight.hpp"

// STD
#include <utility>

using namespace glengine;

// Constructors, assignment operators and destructor

// Default constructor
DirectionalLight::DirectionalLight() noexcept {}

// Parameterized constructor
DirectionalLight::DirectionalLight(const glm::vec3 &color, float intensity,
                                   const glm::vec3 &direction) noexcept
    : BaseLight{color, intensity}, _direction{direction} {}

// Copy constructor (base class)
DirectionalLight::DirectionalLight(const BaseLight &light) noexcept : BaseLight{light} {}

// Copy assignment operator (base class)
DirectionalLight &DirectionalLight::operator=(const BaseLight &light) noexcept {
  this->BaseLight::operator=(light);

  return *this;
}

// Copy constructor
DirectionalLight::DirectionalLight(const DirectionalLight &light) noexcept
    : BaseLight{dynamic_cast<const BaseLight &>(light)}, _direction{light._direction} {}

// Copy assignment operator
DirectionalLight &DirectionalLight::operator=(const DirectionalLight &light) noexcept {
  this->BaseLight::operator=(dynamic_cast<const BaseLight &>(light));

  _direction = light._direction;

  return *this;
}

// Move constructor
DirectionalLight::DirectionalLight(DirectionalLight &&light) noexcept
    : BaseLight{dynamic_cast<BaseLight &&>(light)},
      _direction{std::exchange(light._direction, glm::vec3{})} {}

// Move assignment operator
DirectionalLight &DirectionalLight::operator=(DirectionalLight &&light) noexcept {
  this->BaseLight::operator=(dynamic_cast<BaseLight &&>(light));

  std::swap(_direction, light._direction);

  return *this;
}

// Destructor
DirectionalLight::~DirectionalLight() noexcept {}

// Setters

void DirectionalLight::setDirection(const glm::vec3 &direction) noexcept { _direction = direction; }

// Getters

const glm::vec3 &DirectionalLight::getDirection() const noexcept { return _direction; }

glm::vec3 &DirectionalLight::getDirection() noexcept { return _direction; }
