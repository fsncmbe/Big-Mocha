#pragma once

#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <modules/ecs.hpp>

namespace mocha
{
class Window : public System
{
 public:
  /**
   * @brief Construct a new Window object
   * 
   * @param width screen width
   * @param height screenheight
   * @param title window title
   */
  Window(int width, int height, const std::string& title);

  /**
   * @brief Updates window
   * 
   * @param dt used for physics
   */
  void update(float dt);

  /**
   * @brief Get the Delta object
   * 
   * @return double& 
   */
  double* getDelta();

 private:
  GLFWwindow* glfw_window;

  double current_frame;
  double previous_frame;
  double delta_time;
  double fps;
};
} // mocha