// STD
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

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
constexpr std::chrono::duration kShaderWatcherInterval = 500ms;

// Functions for GLFW
void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processUserInput(GLFWwindow *window);

// Filesystem service functions
QString getAbsolutePathOfExecutableDirectory();
QString getAbsolutePathRelativeToExecutable(const QString &path);

// Shaders functions
unsigned int initShaderProgram(int shaderCount, unsigned int *shaders,
                               unsigned int *shaderTypes);
unsigned int compileShader(unsigned int shader, const QString &source);
void         shaderWatcher(const bool &running, GLFWwindow *window,
                           unsigned int shaderProgram, int shaderCount,
                           unsigned int *shaders, const QString *shaderFileNames);

// Mesh functions
unsigned int initMesh(const float vertices[], int vertexCount,
                      const unsigned int indices[], int triangleCount);
void drawMesh(unsigned int vao, int triangleCount, unsigned int shaderProgram);

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
  // Making current context attached to created window
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
  constexpr unsigned int kShaderCount    = 2;
  unsigned int          *shaders         = new unsigned int[kShaderCount]{};
  QString               *shaderFileNames = new QString[kShaderCount]{
      getAbsolutePathRelativeToExecutable("vertexShader.glsl"),
      getAbsolutePathRelativeToExecutable("fragmentShader.glsl"),
  };
  unsigned int *shaderTypes = new unsigned int[kShaderCount]{
      GL_VERTEX_SHADER,
      GL_FRAGMENT_SHADER,
  };
  // Creating and configuring a shader program
  unsigned int shaderProgram =
      initShaderProgram(kShaderCount, shaders, shaderTypes);
  // Running shaderWatcher thread
  bool        shaderWatcherThreadRunning = true;
  std::thread shaderWatcherThread{
      shaderWatcher,  std::ref(shaderWatcherThreadRunning),
      window,         shaderProgram,
      kShaderCount,   shaders,
      shaderFileNames};

  // Vertices and their indices to make a mesh from triangles
  constexpr float kVertices[] = {
      -0.5f,  0.5f,  0.0f,
      0.5f,  0.5f,  0.0f,
      -0.25f, -0.5f, 0.0f,
      0.25f, -0.5f, 0.0f,
  };
  constexpr unsigned int kIndices[] = {
      0, 1, 2,
      1, 3, 2,
  };

  // Creating and configuring a mesh and getting its VAO
  unsigned int vao =
      initMesh(kVertices, sizeof(kVertices) / 3 / sizeof(float), kIndices,
               sizeof(kIndices) / 3 / sizeof(unsigned int));

  // While the window is open
  while (!glfwWindowShouldClose(window)) {
    // Processing window events
    glfwPollEvents();

    // Processing user input
    processUserInput(window);

    // Clearing background
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Drawing the mesh
    drawMesh(vao, sizeof(kIndices) / 3 / sizeof(unsigned int), shaderProgram);

    // Swapping the front and back buffers
    glfwSwapBuffers(window);
  }

  delete[] shaders;
  delete[] shaderFileNames;
  delete[] shaderTypes;

  // Waiting for shaderWatcher to stop
  shaderWatcherThreadRunning = false;
  shaderWatcherThread.join();
  // Terminating GLFW
  glfwTerminate();
  // Terminating Qt5 Quick app
  app.quit();

  return 0;
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  // Setting viewport position and size relative to the window
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

unsigned int initShaderProgram(int shaderCount, unsigned int *shaders,
                               unsigned int *shaderTypes) {
  // Creating a shader program
  unsigned int shaderProgram = glCreateProgram();

  // For each shader
  for (int i = 0; i < shaderCount; ++i) {
    // Creating a shader
    shaders[i] = glCreateShader(shaderTypes[i]);
    // Attaching shader to shader program
    glAttachShader(shaderProgram, shaders[i]);
  }

  return shaderProgram;
}

unsigned int compileShader(unsigned int shader, const QString &source) {
  // Setting shader's source and compiling it
  const char *shaderSource = source.toLocal8Bit().data();
  glShaderSource(shader, 1, &shaderSource, nullptr);
  glCompileShader(shader);

  // Getting compilation success code
  int success{};
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  // Getting info log length
  int infoLogLength{};
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
  // Getting info log
  char *infoLog = new char[infoLogLength]{};
  glGetShaderInfoLog(shader, infoLogLength, nullptr, infoLog);
  // If compilation have failed
  if (success == GL_FALSE) {
    std::cout << "error: unable to compile shader #" << shader << "\n";
    // If there is an info log
    if (infoLogLength > 0) {
      std::cout << infoLog;
    }
    // If there is no info log
    else {
      std::cout << "no error information";
    }
    std::cout << std::endl;
  }
  // If compilation have succeed and there is an info log
  else if (infoLogLength > 0) {
    std::cout << "info: shader #" << shader << "\n" << infoLog << std::endl;
  }
  delete[] infoLog;

  return shader;
}

void shaderWatcher(const bool &running, GLFWwindow *window,
                   unsigned int shaderProgram, int shaderCount,
                   unsigned int *shaders, const QString *shaderFileNames) {
  // Making current context attached to created window
  glfwMakeContextCurrent(window);

  // Creating an array of last modification times
  qint64 *shaderLastModificationTimes = new qint64[shaderCount]{};

  // While thread is running
  while (running) {
    // No shaders are modified yet
    bool modified = false;

    // For each shader
    for (int i = 0; i < shaderCount; ++i) {
      QFile shaderFile{shaderFileNames[i]};
      if (!shaderFile.open(QFile::ReadOnly | QFile::Text)) {
        std::cout << "error: unable to find "
                  << shaderFileNames[i].toStdString() << std::endl;
        continue;
      }
      QFileInfo shaderFileInfo{shaderFile};
      qint64    shaderModificationTime =
          shaderFileInfo.lastModified().toMSecsSinceEpoch();

      if (shaderModificationTime > shaderLastModificationTimes[i]) {
        // Reading shader source code from the file
        QTextStream shaderFileTextStream{&shaderFile};
        QString     shaderSource{shaderFileTextStream.readAll()};

        // Compiling the shader
        compileShader(shaders[i], shaderSource);

        // Updating last modification time
        shaderLastModificationTimes[i] = shaderModificationTime;

        // The shader is modified
        modified = true;
      }
    }

    // At least one shader is modified
    if (modified) {
      // Linking shader program
      glLinkProgram(shaderProgram);

      // Getting linkage success code
      int success{};
      glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
      // Getting linkage info log length
      int infoLogLength{};
      glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
      // Getting linkage info log
      char *infoLog = new char[infoLogLength]{};
      glGetProgramInfoLog(shaderProgram, infoLogLength, nullptr, infoLog);
      // If linkage have failed
      if (success == GL_FALSE) {
        std::cout << "error: unable to link shaders in program #"
                  << shaderProgram << "\n";
        // If there is an info log
        if (infoLogLength > 0) {
          std::cout << infoLog;
        }
        // If there is no info log
        else {
          std::cout << "no error information";
        }
        std::cout << std::endl;
        // If linkage have succeed and there is an info log
      } else if (infoLogLength > 0) {
        std::cout << "info: shader program #" << shaderProgram << "\n"
                  << infoLog << std::endl;
      }
      delete[] infoLog;
    }

    // Wait some time to approximately follow the time interval
    std::this_thread::sleep_for(kShaderWatcherInterval);
  }

  delete[] shaderLastModificationTimes;
}

unsigned int initMesh(const float vertices[], int vertexCount,
                      const unsigned int indices[], int triangleCount) {
  // Creating VAO, VBO and EBO
  unsigned int vao{};
  glGenVertexArrays(1, &vao);
  unsigned int vbo{};
  glGenBuffers(1, &vbo);
  unsigned int ebo{};
  glGenBuffers(1, &ebo);

  // Binding VAO to bind to it VBO and EBO and then configure them
  glBindVertexArray(vao);

  // Binding and filling VBO
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(float), vertices,
               GL_STATIC_DRAW);

  // Binding and filling EBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               triangleCount * 3 * sizeof(unsigned int), indices,
               GL_STATIC_DRAW);

  // Configuring and enabling VBO's 0th attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                        reinterpret_cast<void *>(0));
  glEnableVertexAttribArray(0);

  // Unbinding configured VAO to use it later
  glBindVertexArray(0);

  return vao;
}

void drawMesh(unsigned int vao, int triangleCount, unsigned int shaderProgram) {
  // Setting specific shader program to use for render
  glUseProgram(shaderProgram);
  // Binding VAO with associated VBO and EBO
  glBindVertexArray(vao);

  // Drawing mesh
  //glDrawArrays(GL_TRIANGLES, 0, 3); // without vertices (EBO) method
  glDrawElements(GL_TRIANGLES, triangleCount * 3, GL_UNSIGNED_INT,
                 0);  // with vertices (EBO) method

  // Unbinding VAO
  glBindVertexArray(0);
}
