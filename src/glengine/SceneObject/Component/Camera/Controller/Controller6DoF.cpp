// Header file
#include "./Controller6DoF.hpp"

// STD
#include <utility>

// GLM
#include <glm/gtx/rotate_vector.hpp>

using namespace glengine;

// Constructors, assignment operators and destructor

// Default constructor
Controller6DoF::Controller6DoF() noexcept {}

// Parameterized constructor
Controller6DoF::Controller6DoF(BaseCamera *camera) noexcept : Controller3DoF{camera} {}

// Copy constructor (base class)
Controller6DoF::Controller6DoF(const Controller3DoF &cameraController) noexcept
    : Controller3DoF{cameraController} {}

// Copy assignment operator (base class)
Controller6DoF &Controller6DoF::operator=(const Controller3DoF &cameraController) noexcept {
  Controller3DoF::operator=(cameraController);

  return *this;
}

// Copy constructor
Controller6DoF::Controller6DoF(const Controller6DoF &cameraController) noexcept
    : Controller3DoF{dynamic_cast<const Controller3DoF &>(cameraController)} {}

// Copy assignment operator
Controller6DoF &Controller6DoF::operator=(const Controller6DoF &cameraController) noexcept {
  Controller3DoF::operator=(dynamic_cast<const Controller3DoF &>(cameraController));

  return *this;
}

// Move constructor
Controller6DoF::Controller6DoF(Controller6DoF &&cameraController) noexcept
    : Controller3DoF{dynamic_cast<Controller3DoF &&>(cameraController)} {}

// Move assignment operator
Controller6DoF &Controller6DoF::operator=(Controller6DoF &&cameraController) noexcept {
  Controller3DoF::operator=(dynamic_cast<Controller3DoF &&>(cameraController));

  return *this;
}

// Destructor
Controller6DoF::~Controller6DoF() noexcept {}

// Other member functions

void Controller6DoF::rotateForward(float angle) {
  // Precision dependent method
  _camera->getRight() = glm::rotate(_camera->getRight(), angle, _camera->getForward());
  _camera->getUp()    = glm::rotate(_camera->getUp(), angle, _camera->getForward());

  _camera->recalculateViewMatrix();
}

void Controller6DoF::rotateRight(float angle) {
  // Precision dependent method
  _camera->getUp()      = glm::rotate(_camera->getUp(), angle, _camera->getRight());
  _camera->getForward() = glm::rotate(_camera->getForward(), angle, _camera->getRight());

  _camera->recalculateViewMatrix();
}

void Controller6DoF::rotateUp(float angle) {
  // Precision dependent method
  _camera->getForward() = glm::rotate(_camera->getForward(), angle, _camera->getUp());
  _camera->getRight()   = glm::rotate(_camera->getRight(), angle, _camera->getUp());

  _camera->recalculateViewMatrix();
}
