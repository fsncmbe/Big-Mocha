#include <iostream>

#include <mocha.hpp>

int main()
{
  std::vector<mocha::System*> systems;
  auto window = new mocha::Window(1920, 1080, "Mocha");
  double* dt = window->getDelta();

  auto res = new mocha::Resource();
  mocha::ResourceHandle txt = res->load("text/d_hello.txt");
  std::cout << txt.uuid << "\n";
  mocha::ResourceHandle t = res->load("text/s_hello.txt");
  std::cout << t.uuid << "\n";

  systems.push_back(window);
  systems.push_back(res);
  int i {0};
  while (true)
  {
    for (mocha::System* s : systems)
    {
      s->update(*dt);
      i++;
      if (i%100000 == 0)
      {
        std::cout << CAST_TEXTFILE(*res->get(txt.uuid)).text << "\n";
        std::cout << CAST_TEXTFILE(*res->get(t.uuid)).text << "\n";
      }
    }
  }
}