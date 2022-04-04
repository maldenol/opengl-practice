// STD
#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

// Qt5
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QTextStream>

// OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// for "ms"
using namespace std::chrono_literals;

// Global constants
constexpr unsigned int          kWidth                 = 800;
constexpr unsigned int          kHeight                = 600;
constexpr std::chrono::duration kRenderCycleInterval   = 16ms;
constexpr std::chrono::duration kShaderWatcherInterval = 500ms;

// Global variables
std::atomic<bool> gShaderWatcherIsRunning = true;
std::mutex        gGLFWContextMutex{};

// Functions for GLFW
void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processUserInput(GLFWwindow *window);

// Filesystem service functions
QString getAbsolutePathOfExecutableDirectory();
QString getAbsolutePathRelativeToExecutable(const QString &path);

// Shaders functions
GLuint initShaderProgram(std::vector<unsigned int>       &shaders,
                         const std::vector<unsigned int> &shaderTypes);
void   compileShader(GLuint shader, const QString &source);
void   shaderWatcher(GLFWwindow *window, GLuint shaderProgram,
                     const std::vector<unsigned int> &shaders,
                     const std::vector<QString>      &shaderFileNames);

// Mesh functions
GLuint initMesh(const std::vector<float>        &vertices,
                const std::vector<unsigned int> &indices);
void   drawMesh(GLuint vao, GLsizei vertexCount, GLuint shaderProgram);

// Main function
int main(int argc, char *argv[]) {
  // Initializing Qt5 Quick application
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication app{argc, argv};

  // Initializing GLFW
  if (glfwInit() == GLFW_FALSE) {
    std::cout << "error: failed to init GLFW" << std::endl;

    // Terminating Qt5 Quick app
    app.quit();

    return -1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Creating a window
  GLFWwindow *window =
      glfwCreateWindow(kWidth, kHeight, "triangle", nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "error: failed to create GLFW window" << std::endl;

    // Terminating GLFW
    glfwTerminate();
    // Terminating Qt5 Quick app
    app.quit();

    return -1;
  }
  // Capturing OpenGL context
  glfwMakeContextCurrent(window);

  // Loading OpenGL function using GLAD
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cout << "error: failed to load OpenGL functions using GLAD"
              << std::endl;

    // Terminating GLFW
    glfwTerminate();
    // Terminating Qt5 Quick app
    app.quit();

    return -1;
  }

  // Setting callback function on window resize
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  // Creating arrays of shaders, according filenames and types
  std::vector<unsigned int> shaders{0, 0};
  std::vector<QString>      shaderFileNames{
      getAbsolutePathRelativeToExecutable("vertexShader.glsl"),
      getAbsolutePathRelativeToExecutable("fragmentShader.glsl"),
  };
  std::vector<unsigned int> shaderTypes{
      GL_VERTEX_SHADER,
      GL_FRAGMENT_SHADER,
  };
  // Creating and configuring a shader program
  GLuint shaderProgram =
      initShaderProgram(std::ref(shaders), std::cref(shaderTypes));
  // Running shaderWatcher thread
  gShaderWatcherIsRunning = true;
  std::thread shaderWatcherThread{shaderWatcher, window, shaderProgram,
                                  std::cref(shaders),
                                  std::cref(shaderFileNames)};

  // Vertices and their indices to make a mesh from triangles
  std::vector<float> vertices{
      -0.5f,  0.5f,  0.0f,
      0.5f,  0.5f,  0.0f,
      -0.25f, -0.5f, 0.0f,
      0.25f, -0.5f, 0.0f,
  };
  std::vector<unsigned int> indices{
      0, 1, 2,
      1, 3, 2,
  };
  // Creating and configuring a mesh and getting its VAO
  GLuint vao = initMesh(std::cref(vertices), std::cref(indices));

  // Releasing OpenGL context
  glfwMakeContextCurrent(nullptr);

  // Creating gGLFWContextMutex unique_lock
  std::unique_lock<std::mutex> gGlfwContextLock{gGLFWContextMutex,
                                                std::defer_lock};

  // Render cycle
  while (true) {
    // Capturing mutex and OpenGL context
    gGlfwContextLock.lock();
    glfwMakeContextCurrent(window);

    // If window should close
    if (glfwWindowShouldClose(window)) break;

    // Processing window events
    glfwPollEvents();

    // Processing user input
    processUserInput(window);

    // Clearing background
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Drawing mesh
    drawMesh(vao, vertices.size(), shaderProgram);

    // Swapping front and back buffers
    glfwSwapBuffers(window);

    // Releasing OpenGL context and mutex
    glfwMakeContextCurrent(nullptr);
    gGlfwContextLock.unlock();

    std::this_thread::sleep_for(kRenderCycleInterval);
  }

  // Waiting for shaderWatcher to stop
  gShaderWatcherIsRunning = false;
  shaderWatcherThread.join();
  // Terminating GLFW
  glfwTerminate();
  // Terminating Qt5 Quick app
  app.quit();

  return 0;
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  // Setting viewport position and size relative to window
  glViewport(0, 0, width, height);
}

void processUserInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    // Making window close
    glfwSetWindowShouldClose(window, true);
    return;
  }

  if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
    // Setting polygon mode for both sides to GL_LINE
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
    // Setting polygon mode for both sides to GL_FILL
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}

QString getAbsolutePathOfExecutableDirectory() {
  return QCoreApplication::applicationDirPath();
}

QString getAbsolutePathRelativeToExecutable(const QString &path) {
  return getAbsolutePathOfExecutableDirectory() + QDir::separator() + path;
}

GLuint initShaderProgram(std::vector<unsigned int>       &shaders,
                         const std::vector<unsigned int> &shaderTypes) {
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

void compileShader(GLuint shader, const QString &source) {
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

void shaderWatcher(GLFWwindow *window, GLuint shaderProgram,
                   const std::vector<unsigned int> &shaders,
                   const std::vector<QString>      &shaderFileNames) {
  size_t shaderCount = shaders.size();

  // If sizes mismatch
  if (shaderCount != shaderFileNames.size()) return;

  // Creating and initializaing an array of last modification times
  std::vector<qint64> shaderLastModificationTimes{};
  shaderLastModificationTimes.assign(shaderCount, 0);

  // Creating gGLFWContextMutex unique_lock
  std::unique_lock<std::mutex> gGlfwContextLock{gGLFWContextMutex,
                                                std::defer_lock};

  // While thread is running
  while (gShaderWatcherIsRunning) {
    // Capturing mutex and OpenGL context
    gGlfwContextLock.lock();
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
        compileShader(shaders[i], shaderSource);

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
    gGlfwContextLock.unlock();

    // Wait some time to approximately follow the time interval
    std::this_thread::sleep_for(kShaderWatcherInterval);
  }
}

GLuint initMesh(const std::vector<float>        &vertices,
                const std::vector<unsigned int> &indices) {
  // Creating VAO, VBO and EBO
  GLuint vao{};
  glGenVertexArrays(1, &vao);
  GLuint vbo{};
  glGenBuffers(1, &vbo);
  GLuint ebo{};
  glGenBuffers(1, &ebo);

  // Binding VAO to bind to it VBO and EBO and then configure them
  glBindVertexArray(vao);

  // Binding and filling VBO
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0],
               GL_STATIC_DRAW);

  // Binding and filling EBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               &indices[0], GL_STATIC_DRAW);

  // Configuring and enabling VBO's 0th attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  // Unbinding configured VAO to use it later
  glBindVertexArray(0);

  return vao;
}

void drawMesh(GLuint vao, GLsizei vertexCount, GLuint shaderProgram) {
  // Setting specific shader program to use for render
  glUseProgram(shaderProgram);
  // Binding VAO with associated VBO and EBO
  glBindVertexArray(vao);

  // Drawing mesh
  //glDrawArrays(GL_TRIANGLES, 0, 3); // without vertices (EBO) method
  glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT,
                 0);  // with vertices (EBO) method

  // Unbinding VAO
  glBindVertexArray(0);
}
