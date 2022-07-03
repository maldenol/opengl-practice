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

// Compiles shader with source code
void compileShader(GLuint shader, const QString &source, const QString &filename);

// Compiles shader with source code
void compileShader(GLuint shader, const std::string &source, const std::string &filename);

// Creates shader program compiling and linking given shaders
GLuint createShaderProgram(const std::vector<GLenum>      &shaderTypes,
                           const std::vector<std::string> &shaderSources,
                           const std::vector<std::string> &shaderFilenames);

// Watches for modifications of shader files (must be run in thread)
void shaderWatcher(const std::atomic<bool> &isRunning, std::atomic<bool> &shadersAreRecompiled,
                   GLFWwindow *window, std::mutex &glfwContextMutex, GLuint shaderProgram,
                   const std::vector<GLenum>  &shaderTypes,
                   const std::vector<QString> &shaderFilenames);

}  // namespace glservice

#endif
