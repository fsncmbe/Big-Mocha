#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <modules/ecs.hpp>
#include <common/basics.hpp>

namespace mocha
{

struct Vertex
{
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 tex_pos;
  bool operator==(const Vertex& other);
};

struct Shader
{
  int id;
};

struct Model
{
  int indices_count;
  unsigned int vao;
};

struct cGraphics
{
  Model model;
};

class Graphics : public System
{
 public:
  
  Graphics();

  void update(float dt);

 private:
  
};
} // mocha