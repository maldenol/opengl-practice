#ifndef GLSERVICE_SHADER_HPP
#define GLSERVICE_SHADER_HPP

// STD
#include <atomic>
#include <mutex>
#include <vector>

// Qt5
#include <QString>

// OpenGL
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace glservice {

// Initializes shader program with shaders
GLuint initShaderProgram(std::vector<GLuint>       &shaders,
                         const std::vector<GLuint> &shaderTypes);

// Compiles shader with source code
void compileShader(GLuint shader, const QString &source);

// Watches for modifications of shader files (must be run in thread)
void shaderWatcher(const std::atomic<bool> &isRunning, GLFWwindow *window,
                   std::mutex &glfwContextMutex, GLuint shaderProgram,
                   const std::vector<GLuint>  &shaders,
                   const std::vector<QString> &shaderFileNames);

}  // namespace glservice

#endif
