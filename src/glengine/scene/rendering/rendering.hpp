#ifndef GLENGINE_SCENE_RENDERING_RENDERING_HPP
#define GLENGINE_SCENE_RENDERING_RENDERING_HPP

// "glengine" internal library
#include "../sceneObject.hpp"
#include "../camera/camera.hpp"

namespace glengine {

// Renders scene object
void renderSceneObject(const SceneObject &sceneObject, const BaseCamera &camera);

}  // namespace glengine

#endif
