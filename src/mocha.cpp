#include <iostream>

#include <mocha.hpp>

int main()
{
  std::vector<mocha::System*> systems;
  auto window = new mocha::Window(1920, 1080, "Mocha");
  double* dt = window->getDelta();
  systems.push_back(window);

  auto res = new mocha::Resource();
  mocha::ResourceHandle txt = res->load("text/s_hello.txt");

  auto a = CAST_STR(*res->get(txt.uuid));
  std::cout << a << "\n";

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