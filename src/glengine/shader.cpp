// Header file
#include <glengine/shader.hpp>

// STD
#include <chrono>
#include <iostream>
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

// Initializes shader program with shaders
GLuint glengine::initShaderProgram(std::vector<GLuint>       &shaders,
                                   const std::vector<GLuint> &shaderTypes) {
  // Getting shader count
  size_t shaderCount = shaders.size();

  // If sizes mismatch
  if (shaderCount != shaderTypes.size()) return 0;

  // Creating a shader program
  GLuint shaderProgram = glCreateProgram();

  // For each shader
  for (size_t i = 0; i < shaderCount; ++i) {
    // Creating a shader
    shaders[i] = glCreateShader(shaderTypes[i]);
    // Attaching shader to shader program
    glAttachShader(shaderProgram, shaders[i]);
  }

  return shaderProgram;
}

// Compiles shader with source code
void glengine::compileShader(GLuint shader, const QString &source) {
  // Setting shader's source and compiling it
  const char *shaderSource = source.toLocal8Bit().data();
  glShaderSource(shader, 1, &shaderSource, nullptr);
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
  // If compilation have succeed and there is an info log
  else if (infoLogLength > 0) {
    std::cout << "info: shader #" << shader << "\n" << &infoLog[0] << std::endl;
  }
}

// Watches for modifications of shader files (must be run in thread)
void glengine::shaderWatcher(const std::atomic<bool> &isRunning,
                             GLFWwindow *window, std::mutex &glfwContextMutex,
                             GLuint                      shaderProgram,
                             const std::vector<GLuint>  &shaders,
                             const std::vector<QString> &shaderFileNames) {
  // Getting shader count
  size_t shaderCount = shaders.size();

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

    // No shaders are modified yet
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
        // Reading shader source code from file
        QTextStream shaderFileTextStream{&shaderFile};
        QString     shaderSource{shaderFileTextStream.readAll()};

        // Compiling shader
        glengine::compileShader(shaders[i], shaderSource);

        // Updating last modification time
        shaderLastModificationTimes[i] = shaderModificationTime;

        // Shader is modified
        modified = true;
      }
    }

    // At least one shader is modified
    if (modified) {
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
        // If linkage have succeed and there is an info log
      } else if (infoLogLength > 0) {
        std::cout << "info: shader program #" << shaderProgram << "\n"
                  << &infoLog[0] << std::endl;
      }
    }

    // Releasing OpenGL context and mutex
    glfwMakeContextCurrent(nullptr);
    glfwContextLock.unlock();

    // Wait some time to approximately follow the time interval
    std::this_thread::sleep_for(kShaderWatcherInterval);
  }
}