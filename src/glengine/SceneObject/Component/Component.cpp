// Header file
#include "./Component.hpp"

// STD
#include <utility>

using namespace glengine;

// Constructors, assignment operators and destructor

// Default constructor
Component::Component() noexcept {}

// Parameterized constructor
Component::Component(ComponentType type) noexcept : _type{type} {}

// Copy constructor
Component::Component(const Component &component) noexcept : _type{component._type} {}

// Copy assignment operator
Component &Component::operator=(const Component &component) noexcept {
  _type = component._type;

  return *this;
}

// Move constructor
Component::Component(Component &&component) noexcept
    : _type{std::exchange(component._type, ComponentType{})} {}

// Move assignment operator
Component &Component::operator=(Component &&component) noexcept {
  std::swap(_type, component._type);

  return *this;
}

// Destructor
Component::~Component() noexcept {}

// Setters

void Component::setType(ComponentType type) noexcept { _type = type; }

// Getters

ComponentType Component::getType() const noexcept { return _type; }

const ComponentType &Component::getType() noexcept { return _type; }
