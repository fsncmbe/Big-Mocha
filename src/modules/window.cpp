#include <modules/window.hpp>
#include <iostream>

#ifdef LINUX
  #include <unistd.h>
#endif

#ifdef WINDOWS
  #include <windows.h>
#endif

namespace mocha
{

Window::Window(int width, int height, const std::string& title)
{
  if (!glfwInit())
    glfwTerminate();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfw_window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

  if (!glfw_window)
    glfwTerminate();

  glfwMakeContextCurrent(glfw_window);

  if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    glfwTerminate();

  glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glEnable(GL_DEPTH_TEST);
}

void Window::update(float dt)
{
  current_frame = glfwGetTime();
  delta_time = current_frame - previous_frame;
  previous_frame = current_frame;

  if (delta_time < fps)
  {
    #ifdef LINUX
      sleep(fps - delta_time);
    #endif
    delta_time = fps;
  }

  glfwPollEvents();

  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

double* Window::getDelta()
{
  return &delta_time;
}
}