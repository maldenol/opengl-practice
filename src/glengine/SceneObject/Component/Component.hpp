#ifndef GLENGINE_SCENEOBJECT_COMPONENT_COMPONENT_HPP
#define GLENGINE_SCENEOBJECT_COMPONENT_COMPONENT_HPP

namespace glengine {

// Component type enum class
enum class ComponentType {
  None,
  Camera,
  Light,
  Mesh,
};

// Component class
class Component {
 private:
  ComponentType _type{};

  // Constructors, assignment operators and destructor
  Component() noexcept;

 protected:
  // Constructors, assignment operators and destructor
  Component(ComponentType type) noexcept;
  Component(const Component &component) noexcept;
  Component &operator=(const Component &component) noexcept;
  Component(Component &&component) noexcept;
  Component &operator=(Component &&component) noexcept;

 public:
  // Constructors, assignment operators and destructor
  virtual ~Component() noexcept;

  // Setters
  void setType(ComponentType type) noexcept;

  // Getters
  ComponentType        getType() const noexcept;
  const ComponentType &getType() noexcept;
};

}  // namespace glengine

#endif
