// Header file
#include <glservice/shader.hpp>

// STD
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

// Qt5
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

// for "ms"
using namespace std::chrono_literals;

// Global constants
constexpr std::chrono::duration kShaderWatcherInterval = 500ms;

// Compiles shader with source code
void glservice::compileShader(GLuint shader, const QString &source) {
  // Setting shader's source and compiling it
  std::string shaderSourceString = source.toStdString();
  const char *shaderSourceBuffer = shaderSourceString.c_str();
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
    std::cout << "error: unable to compile shader #" << shader << "\n";
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

// Watches for modifications of shader files (must be run in thread)
void glservice::shaderWatcher(const std::atomic<bool> &isRunning,
                              std::atomic<bool>       &shadersAreRecompiled,
                              GLFWwindow *window, std::mutex &glfwContextMutex,
                              GLuint                      shaderProgram,
                              const std::vector<GLenum>  &shaderTypes,
                              const std::vector<QString> &shaderFileNames) {
  // Getting shader count
  size_t shaderCount = shaderTypes.size();

  // If sizes mismatch
  if (shaderCount != shaderFileNames.size()) return;

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
      QFile shaderFile{shaderFileNames[i]};
      if (!shaderFile.open(QFile::ReadOnly | QFile::Text)) {
        std::cout << "error: unable to find "
                  << shaderFileNames[i].toStdString() << std::endl;
        continue;
      }
      QFileInfo shaderFileInfo{shaderFile};
      qint64    shaderModificationTime =
          shaderFileInfo.lastModified().toMSecsSinceEpoch();

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
            QFile innerShaderFile{shaderFileNames[j]};
            if (!innerShaderFile.open(QFile::ReadOnly | QFile::Text)) {
              std::cout << "error: unable to find "
                        << shaderFileNames[j].toStdString() << std::endl;
              continue;
            }
            QTextStream innerShaderFileTextStream{&innerShaderFile};
            QString     shaderSource{innerShaderFileTextStream.readAll()};

            // Creating shader
            shaders[j] = glCreateShader(shaderTypes[j]);
            // Attaching shader to shader program
            glAttachShader(shaderProgram, shaders[j]);
            // Compiling shader
            glservice::compileShader(shaders[j], shaderSource);
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
          glGetProgramInfoLog(shaderProgram, infoLogLength, nullptr,
                              &infoLog[0]);
          // If linkage have failed
          if (success == GL_FALSE) {
            std::cout << "error: unable to link shaders in program #"
                      << shaderProgram << "\n";
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
            // Deaching shader to shader program
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