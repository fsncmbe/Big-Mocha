#include <iostream>

#include <modules/graphics.hpp>

namespace mocha
{
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