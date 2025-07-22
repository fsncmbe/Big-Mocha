#include <iostream>

#include <mocha.hpp>

int main()
{
  std::vector<mocha::System*> systems;
  auto window = new mocha::Window(1920, 1080, "Mocha");
  double* dt = window->getDelta();
  systems.push_back(window);

  int i {0};
  while (true)
  {
    for (mocha::System* s : systems)
    {
      s->update(*dt);
      i++;
    }
    if (i == 200) break;
  }
}