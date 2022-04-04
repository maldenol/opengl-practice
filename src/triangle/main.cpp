#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Constants for window and viewport size
constexpr int kWidth  = 800;
constexpr int kHeight = 600;

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processUserInput(GLFWwindow *window);

std::string  getAbsolutePathOfExecutableDirectory();
unsigned int compileShader(unsigned int       shaderType,
                           const std::string &filename);
unsigned int createShaderProgram();

unsigned int initMesh(const float vertices[], unsigned int vertexCount,
                      const unsigned int indices[], unsigned int triangleCount);
void         drawMesh(unsigned int vao, unsigned int triangleCount,
                      unsigned int shaderProgram);

int main() {
  // Initializing GLFW
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Creating a window
  GLFWwindow *window =
      glfwCreateWindow(kWidth, kHeight, "lesson1", nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "error: failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // Initializing GLAD
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cout << "error: failed to load glfwGetProcAddress function"
              << std::endl;
    glfwTerminate();
    return -1;
  }

  // Setting callback function on window resize
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  // Creating a shader program
  unsigned int shaderProgram = createShaderProgram();

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

  // Initializing a mesh and getting its VAO
  unsigned int vao =
      initMesh(kVertices, sizeof(kVertices) / 3 / sizeof(float), kIndices,
               sizeof(kIndices) / 3 / sizeof(unsigned int));

  // While the window is open
  while (!glfwWindowShouldClose(window)) {
    // Processing window events
    glfwPollEvents();

    processUserInput(window);

    // Clearing background
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    drawMesh(vao, sizeof(kIndices) / 3 / sizeof(unsigned int), shaderProgram);

    // Swapping the front and back buffers
    glfwSwapBuffers(window);
  }

  // Closing the window
  glfwTerminate();
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

unsigned int compileShader(unsigned int       shaderType,
                           const std::string &filename) {
  // Opening shader file
  std::ifstream shaderFile(getAbsolutePathOfExecutableDirectory() + filename);
  if (!shaderFile.is_open()) {
    std::cout << "error: unable to find " << filename << std::endl;
  }

  // Reading shader source code from the file
  std::string shaderSourceString{};
  std::string sourceLine{};
  while (std::getline(shaderFile, sourceLine)) {
    shaderSourceString += sourceLine + "\n";
  };
  const char *shaderSource = shaderSourceString.c_str();

  // Creating a shader, setting its source and compiling it
  unsigned int shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, &shaderSource, nullptr);
  glCompileShader(shader);

  int success = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(shader, 512, nullptr, infoLog);
    std::cout << "error: unable to compile " << filename << "\n" << infoLog;
  }

  return shader;
}

unsigned int createShaderProgram() {
  // Getting compiled vertex and fragment shaders
  unsigned int vertexShader =
      compileShader(GL_VERTEX_SHADER, "vertexShader.glsl");
  unsigned int fragmentShader =
      compileShader(GL_FRAGMENT_SHADER, "fragmentShader.glsl");

  // Creating a shader program
  unsigned int shaderProgram = glCreateProgram();

  // Attaching shaders to program and linking them
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  int success = 0;
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
    std::cout << "error: unable to link shaders\n" << infoLog;
  }

  // Deleting shaders useless for now
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return shaderProgram;
}

unsigned int initMesh(const float vertices[], unsigned int vertexCount,
                      const unsigned int indices[],
                      unsigned int       triangleCount) {
  // Creating VAO, VBO and EBO
  unsigned int vao = 0;
  glGenVertexArrays(1, &vao);
  unsigned int vbo = 0;
  glGenBuffers(1, &vbo);
  unsigned int ebo = 0;
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

void drawMesh(unsigned int vao, unsigned int triangleCount,
              unsigned int shaderProgram) {
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

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <libloaderapi.h>
#elif defined(__linux__) || defined(__unix__) || defined(__posix__)
#include <unistd.h>
#endif
std::string getAbsolutePathOfExecutableDirectory() {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
  wchar_t path[512] = {0};
  GetModuleFileNameW(nullptr, path, 512);
  return std::filesystem::path(path).parent_path().string() +
         std::filesystem::path::preferred_separator;
#elif defined(__linux__) || defined(__unix__) || defined(__posix__)
  char    path[512];
  ssize_t count = readlink("/proc/self/exe", path, 512);
  return std::filesystem::path(std::string(path, (count > 0) ? count : 0))
             .parent_path()
             .string() +
         std::filesystem::path::preferred_separator;
#else
  std::cout << "error: OS is not supported" << std::endl;
  return std::string{};
#endif
}
