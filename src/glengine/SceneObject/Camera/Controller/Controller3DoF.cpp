// Header file
#include "./Controller3DoF.hpp"

// STD
#include <utility>

using namespace glengine;

// Constructors, assignment operators and destructor

// Default constructor
Controller3DoF::Controller3DoF() noexcept {}

// Parameterized constructor
Controller3DoF::Controller3DoF(BaseCamera *camera) noexcept : _camera{camera} {}

// Copy constructor
Controller3DoF::Controller3DoF(const Controller3DoF &cameraController) noexcept
    : _camera{cameraController._camera} {}

// Copy assignment operator
Controller3DoF &Controller3DoF::operator=(const Controller3DoF &cameraController) noexcept {
  _camera = cameraController._camera;

  return *this;
}

// Move constructor
Controller3DoF::Controller3DoF(Controller3DoF &&cameraController) noexcept
    : _camera{std::exchange(cameraController._camera, nullptr)} {}

// Move assignment operator
Controller3DoF &Controller3DoF::operator=(Controller3DoF &&cameraController) noexcept {
  std::swap(_camera, cameraController._camera);

  return *this;
}

// Destructor
Controller3DoF::~Controller3DoF() noexcept {}

// Setters

void Controller3DoF::setCamera(BaseCamera *camera) noexcept { _camera = camera; }

// Getters

const BaseCamera *Controller3DoF::getCamera() const noexcept { return _camera; }

BaseCamera *Controller3DoF::getCamera() noexcept { return _camera; }

// Other member functions

void Controller3DoF::move(const glm::vec3 &deltaPos) noexcept {
  _camera->getPosition() += deltaPos;

  _camera->recalculateViewMatrix();
}

void Controller3DoF::moveForward(float distance) noexcept {
  _camera->getPosition() += _camera->getForward() * distance;

  _camera->recalculateViewMatrix();
}

void Controller3DoF::moveRight(float distance) noexcept {
  _camera->getPosition() += _camera->getRight() * distance;

  _camera->recalculateViewMatrix();
}

void Controller3DoF::moveUp(float distance) noexcept {
  _camera->getPosition() += _camera->getUp() * distance;

  _camera->recalculateViewMatrix();
}
