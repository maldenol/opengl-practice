// Header file
#include "./shader.hpp"

// STD
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

// Qt5
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QTextStream>

// for "ms"
using namespace std::chrono_literals;

// Global constants
static constexpr std::chrono::duration kShaderWatcherInterval = 500ms;

// Compiles shader with source code
void glengine::compileShader(GLuint shader, const std::string &source,
                             const std::string &filename) {
  // Setting shader's source and compiling it
  const char *shaderSourceBuffer = source.c_str();
  glShaderSource(shader, 1, &shaderSourceBuffer, nullptr);
  glCompileShader(shader);

  // Getting compilation success code
  GLint success{};
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  // Getting info log length
  GLint infoLogLength{};
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
  // Getting info log
  std::vector<char> infoLog{};
  infoLog.resize(infoLogLength + 1);
  glGetShaderInfoLog(shader, infoLogLength, nullptr, &infoLog[0]);
  // If compilation have failed
  if (success == GL_FALSE) {
    std::cout << "error: unable to compile shader #" << shader << " from " << filename << "\n";
    // If there is an info log
    if (infoLogLength > 0) {
      std::cout << &infoLog[0];
    }
    // If there is no info log
    else {
      std::cout << "no error information";
    }
    std::cout << std::endl;
  }
}

// Creates shader program compiling and linking given shaders
GLuint glengine::createShaderProgram(const std::vector<GLenum>      &shaderTypes,
                                     const std::vector<std::string> &shaderSources,
                                     const std::vector<std::string> &shaderFilenames) {
  GLuint shaderProgram = glCreateProgram();

  // Getting shader count
  size_t shaderCount = shaderTypes.size();

  // If sizes mismatch
  if (shaderTypes.size() != shaderSources.size()) {
    return 0;
  }

  // Creating shader list
  std::vector<GLuint> shaders{};
  shaders.assign(shaderCount, 0);

  // Creating, attaching to shader program and compiling each shader
  for (size_t i = 0; i < shaderCount; ++i) {
    // Creating shader
    shaders[i] = glCreateShader(shaderTypes[i]);
    // Attaching shader to shader program
    glAttachShader(shaderProgram, shaders[i]);
    // Compiling shader
    compileShader(shaders[i], shaderSources[i], shaderFilenames[i]);
  }

  // Linking shader program
  glLinkProgram(shaderProgram);

  // Getting linkage success code
  GLint success{};
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  // Getting linkage info log length
  GLint infoLogLength{};
  glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
  // Getting linkage info log
  std::vector<char> infoLog{};
  infoLog.resize(infoLogLength + 1);
  glGetProgramInfoLog(shaderProgram, infoLogLength, nullptr, &infoLog[0]);
  // If linkage have failed
  if (success == GL_FALSE) {
    std::cout << "error: unable to link shaders in program #" << shaderProgram << "\n";
    // If there is an info log
    if (infoLogLength > 0) {
      std::cout << &infoLog[0];
    }
    // If there is no info log
    else {
      std::cout << "no error information";
    }
    std::cout << std::endl;
  }

  // Detaching from shader program and deleting each shader
  for (size_t i = 0; i < shaderCount; ++i) {
    // Detaching shader to shader program
    glDetachShader(shaderProgram, shaders[i]);
    // Deleting shader
    glDeleteShader(shaders[i]);
  }

  // If linkage have failed
  if (success == GL_FALSE) {
    // Deleting shader program
    glDeleteProgram(shaderProgram);

    return 0;
  }

  // Returning shader program
  return shaderProgram;
}

// Watches for modifications of shader files (must be run in thread)
void glengine::shaderWatcher(const std::atomic<bool> &isRunning,
                             std::atomic<bool> &shadersAreRecompiled, GLFWwindow *window,
                             std::mutex &glfwContextMutex, GLuint shaderProgram,
                             const std::vector<GLenum>      &shaderTypes,
                             const std::vector<std::string> &shaderFilenames) {
  // Getting shader count
  size_t shaderCount = shaderTypes.size();

  // If sizes mismatch
  if (shaderCount != shaderFilenames.size()) return;

  // Creating and initializaing a vector of last modification times
  std::vector<qint64> shaderLastModificationTimes{};
  shaderLastModificationTimes.assign(shaderCount, 0);

  // While thread is running
  while (isRunning) {
    // Capturing mutex and OpenGL context
    std::unique_lock glfwContextLock{glfwContextMutex};
    glfwMakeContextCurrent(window);

    // No shader is modified
    bool modified = false;

    // For each shader
    for (size_t i = 0; i < shaderCount; ++i) {
      QFile shaderFile{QString{shaderFilenames[i].c_str()}};
      if (!shaderFile.open(QFile::ReadOnly | QFile::Text)) {
        std::cout << "error: unable to find " << shaderFilenames[i] << std::endl;
        break;
      }
      QFileInfo shaderFileInfo{shaderFile};
      qint64    shaderModificationTime = shaderFileInfo.lastModified().toMSecsSinceEpoch();

      // If shader was modified
      if (shaderModificationTime > shaderLastModificationTimes[i]) {
        // Updating last modification time
        shaderLastModificationTimes[i] = shaderModificationTime;

        // If it is the first modified shader
        if (!modified) {
          // At least on shader is modified
          modified = true;

          // Creating shader list
          std::vector<GLuint> shaders{};
          shaders.assign(shaderCount, 0);

          // For each shader
          for (size_t j = 0; j < shaderCount; ++j) {
            // Reading shader source code from file
            QFile innerShaderFile{QString{shaderFilenames[j].c_str()}};
            if (!innerShaderFile.open(QFile::ReadOnly | QFile::Text)) {
              std::cout << "error: unable to find " << shaderFilenames[j] << std::endl;
              continue;
            }
            QTextStream innerShaderFileTextStream{&innerShaderFile};
            QString     shaderSource{innerShaderFileTextStream.readAll()};

            // Creating shader
            shaders[j] = glCreateShader(shaderTypes[j]);
            // Attaching shader to shader program
            glAttachShader(shaderProgram, shaders[j]);
            // Compiling shader
            compileShader(shaders[j], shaderSource.toStdString(), shaderFilenames[j]);
          }

          // Linking shader program
          glLinkProgram(shaderProgram);

          // Getting linkage success code
          GLint success{};
          glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
          // Getting linkage info log length
          GLint infoLogLength{};
          glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
          // Getting linkage info log
          std::vector<char> infoLog{};
          infoLog.resize(infoLogLength + 1);
          glGetProgramInfoLog(shaderProgram, infoLogLength, nullptr, &infoLog[0]);
          // If linkage have failed
          if (success == GL_FALSE) {
            std::cout << "error: unable to link shaders in program #" << shaderProgram << "\n";
            // If there is an info log
            if (infoLogLength > 0) {
              std::cout << &infoLog[0];
            }
            // If there is no info log
            else {
              std::cout << "no error information";
            }
            std::cout << std::endl;
          }

          // For each shader
          for (size_t j = 0; j < shaderCount; ++j) {
            // Detaching shader to shader program
            glDetachShader(shaderProgram, shaders[j]);
            // Deleting shader
            glDeleteShader(shaders[j]);
          }

          // Notifying that shaders are recompiled
          shadersAreRecompiled = true;
        }
      }
    }

    // Releasing OpenGL context and mutex
    glfwMakeContextCurrent(nullptr);
    glfwContextLock.unlock();

    // Wait some time to approximately follow the time interval
    std::this_thread::sleep_for(kShaderWatcherInterval);
  }
}
