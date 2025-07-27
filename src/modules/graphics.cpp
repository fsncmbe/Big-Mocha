#include <iostream>

#include <modules/graphics.hpp>

namespace mocha
{

bool Vertex::operator==(const Vertex& other)
{
  return position == other.position && normal == other.normal && tex_pos == other.tex_pos;
}

Graphics::Graphics()
{

}

void Graphics::update(float dt)
{
  const auto& v = view<cStance, cGraphics>();

  for (int id : v)
  {
    cStance* st = get<cStance>(id);
    cGraphics* gr = get<cGraphics>(id);
  }
}
}