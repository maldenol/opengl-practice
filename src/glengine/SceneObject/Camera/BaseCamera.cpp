// Header file
#include "./BaseCamera.hpp"

// STD
#include <utility>

// GLM
#include <glm/gtc/matrix_transform.hpp>

using namespace glengine;

// Constructors, assignment operators and destructor

// Default constructor
BaseCamera::BaseCamera() noexcept {}

// Parameterized constructor
BaseCamera::BaseCamera(const glm::vec3 &pos, const glm::vec3 &worldUp,
                       const glm::vec3 &lookDir) noexcept
    : _pos{pos}, _worldUp{worldUp} {
  look(lookDir);
}

// Copy constructor
BaseCamera::BaseCamera(const BaseCamera &camera) noexcept
    : _pos{camera._pos},
      _worldUp{camera._worldUp},
      _forward{camera._forward},
      _right{camera._right},
      _up{camera._up} {}

// Copy assignment operator
BaseCamera &BaseCamera::operator=(const BaseCamera &camera) noexcept {
  _pos     = camera._pos;
  _worldUp = camera._worldUp;
  _forward = camera._forward;
  _right   = camera._right;
  _up      = camera._up;

  return *this;
}

// Move constructor
BaseCamera::BaseCamera(BaseCamera &&camera) noexcept
    : _pos{std::exchange(camera._pos, glm::vec3{})},
      _worldUp{std::exchange(camera._worldUp, glm::vec3{})},
      _forward{std::exchange(camera._forward, glm::vec3{})},
      _right{std::exchange(camera._right, glm::vec3{})},
      _up{std::exchange(camera._up, glm::vec3{})} {}

// Move assignment operator
BaseCamera &BaseCamera::operator=(BaseCamera &&camera) noexcept {
  std::swap(_pos, camera._pos);
  std::swap(_worldUp, camera._worldUp);
  std::swap(_forward, camera._forward);
  std::swap(_right, camera._right);
  std::swap(_up, camera._up);

  return *this;
}

// Destructor
BaseCamera::~BaseCamera() noexcept {}

// Setters

void BaseCamera::setPosition(const glm::vec3 &pos) noexcept { _pos = pos; }

void BaseCamera::setWorldUp(const glm::vec3 &worldUp) noexcept {
  _worldUp = glm::normalize(worldUp);
}

void BaseCamera::setForward(const glm::vec3 &forward) noexcept { _forward = forward; }

void BaseCamera::setRight(const glm::vec3 &right) noexcept { _right = right; }

void BaseCamera::setUp(const glm::vec3 &up) noexcept { _up = up; }

// Getters

const glm::vec3 &BaseCamera::getPosition() const noexcept { return _pos; }

glm::vec3 &BaseCamera::getPosition() noexcept { return _pos; }

const glm::vec3 &BaseCamera::getWorldUp() const noexcept { return _worldUp; }

glm::vec3 &BaseCamera::getWorldUp() noexcept { return _worldUp; }

const glm::vec3 &BaseCamera::getForward() const noexcept { return _forward; }

glm::vec3 &BaseCamera::getForward() noexcept { return _forward; }

const glm::vec3 &BaseCamera::getRight() const noexcept { return _right; }

glm::vec3 &BaseCamera::getRight() noexcept { return _right; }

const glm::vec3 &BaseCamera::getUp() const noexcept { return _up; }

glm::vec3 &BaseCamera::getUp() noexcept { return _up; }

// Other member functions

void BaseCamera::look(const glm::vec3 &look) noexcept {
  _forward = glm::normalize(look);
  _right   = glm::cross(_forward, _worldUp);
  _up      = glm::cross(_right, _forward);
}

void BaseCamera::lookAt(const glm::vec3 &lookAt) noexcept {
  _forward = glm::normalize(lookAt - _pos);
  _right   = glm::cross(_forward, _worldUp);
  _up      = glm::cross(_right, _forward);
}

glm::mat4 BaseCamera::getViewMatrix() const noexcept {
  return glm::lookAt(_pos, _pos + _forward, _up);
}
