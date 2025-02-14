// Header file
#include "./BaseCamera.hpp"

// STD
#include <utility>

// GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>

using namespace glengine;

// Constructors, assignment operators and destructor

// Default constructor
BaseCamera::BaseCamera() noexcept : Component{ComponentType::Camera} { recalculateViewMatrix(); }

// Parameterized constructor
BaseCamera::BaseCamera(const glm::vec3 &position, const glm::vec3 &worldUp,
                       const glm::vec3 &lookDir) noexcept
    : Component{ComponentType::Camera}, _position{position}, _worldUp{worldUp} {
  look(lookDir);
}

// Copy constructor
BaseCamera::BaseCamera(const BaseCamera &camera) noexcept
    : Component{dynamic_cast<const Component &>(camera)},
      _position{camera._position},
      _worldUp{camera._worldUp},
      _forward{camera._forward},
      _right{camera._right},
      _up{camera._up},
      _viewMatrix{camera._viewMatrix},
      _projectionMatrix{camera._projectionMatrix} {}

// Copy assignment operator
BaseCamera &BaseCamera::operator=(const BaseCamera &camera) noexcept {
  Component::operator=(dynamic_cast<const Component &>(camera));

  _position         = camera._position;
  _worldUp          = camera._worldUp;
  _forward          = camera._forward;
  _right            = camera._right;
  _up               = camera._up;
  _viewMatrix       = camera._viewMatrix;
  _projectionMatrix = camera._projectionMatrix;

  return *this;
}

// Move constructor
BaseCamera::BaseCamera(BaseCamera &&camera) noexcept
    : Component{dynamic_cast<Component &&>(camera)},
      _position{std::exchange(camera._position, glm::vec3{})},
      _worldUp{std::exchange(camera._worldUp, glm::vec3{})},
      _forward{std::exchange(camera._forward, glm::vec3{})},
      _right{std::exchange(camera._right, glm::vec3{})},
      _up{std::exchange(camera._up, glm::vec3{})},
      _viewMatrix{std::exchange(camera._viewMatrix, glm::mat4{})},
      _projectionMatrix{std::exchange(camera._projectionMatrix, glm::mat4{})} {}

// Move assignment operator
BaseCamera &BaseCamera::operator=(BaseCamera &&camera) noexcept {
  Component::operator=(dynamic_cast<Component &&>(camera));

  std::swap(_position, camera._position);
  std::swap(_worldUp, camera._worldUp);
  std::swap(_forward, camera._forward);
  std::swap(_right, camera._right);
  std::swap(_up, camera._up);
  std::swap(_viewMatrix, camera._viewMatrix);
  std::swap(_projectionMatrix, camera._projectionMatrix);

  return *this;
}

// Destructor
BaseCamera::~BaseCamera() noexcept {}

// Setters

void BaseCamera::setPosition(const glm::vec3 &position) noexcept {
  _position = position;

  recalculateViewMatrix();
}

void BaseCamera::setWorldUp(const glm::vec3 &worldUp) noexcept {
  _worldUp = glm::normalize(worldUp);

  recalculateViewMatrix();
}

void BaseCamera::setForward(const glm::vec3 &forward) noexcept {
  _forward = forward;

  recalculateViewMatrix();
}

void BaseCamera::setRight(const glm::vec3 &right) noexcept {
  _right = right;

  recalculateViewMatrix();
}

void BaseCamera::setUp(const glm::vec3 &up) noexcept {
  _up = up;

  recalculateViewMatrix();
}

void BaseCamera::setViewMatrix(const glm::mat4 &viewMatrix) noexcept { _viewMatrix = viewMatrix; }

void BaseCamera::setProjectionMatrix(const glm::mat4 &projectionMatrix) noexcept {
  _projectionMatrix = projectionMatrix;
}

// Getters

const glm::vec3 &BaseCamera::getPosition() const noexcept { return _position; }

glm::vec3 &BaseCamera::getPosition() noexcept { return _position; }

const glm::vec3 &BaseCamera::getWorldUp() const noexcept { return _worldUp; }

glm::vec3 &BaseCamera::getWorldUp() noexcept { return _worldUp; }

const glm::vec3 &BaseCamera::getForward() const noexcept { return _forward; }

glm::vec3 &BaseCamera::getForward() noexcept { return _forward; }

const glm::vec3 &BaseCamera::getRight() const noexcept { return _right; }

glm::vec3 &BaseCamera::getRight() noexcept { return _right; }

const glm::vec3 &BaseCamera::getUp() const noexcept { return _up; }

glm::vec3 &BaseCamera::getUp() noexcept { return _up; }

const glm::mat4 &BaseCamera::getViewMatrix() const noexcept { return _viewMatrix; }

glm::mat4 &BaseCamera::getViewMatrix() noexcept { return _viewMatrix; }

const glm::mat4 &BaseCamera::getProjectionMatrix() const noexcept { return _projectionMatrix; }

glm::mat4 &BaseCamera::getProjectionMatrix() noexcept { return _projectionMatrix; }

// Other member functions

void BaseCamera::recalculateViewMatrix() noexcept {
  _viewMatrix = glm::lookAt(_position, _position + _forward, _up);
}

void BaseCamera::look(const glm::vec3 &look) noexcept {
  _forward = glm::normalize(look);
  _right   = glm::cross(_forward, _worldUp);
  _up      = glm::cross(_right, _forward);

  recalculateViewMatrix();
}

void BaseCamera::lookAt(const glm::vec3 &lookAt) noexcept {
  _forward = glm::normalize(lookAt - _position);
  _right   = glm::cross(_forward, _worldUp);
  _up      = glm::cross(_right, _forward);

  recalculateViewMatrix();
}
