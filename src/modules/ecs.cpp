#include <modules/ecs.hpp>

namespace mocha
{
int create()
{
  return World::get().next();
}

void remove(int id)
{
  for (auto& [_, s] : World::get().view())
  {
    HandleMap<std::any>* _map = static_cast<HandleMap<std::any>*>(s);
    if (_map->has(id))
      _map->remove(id);
  }
  World::get().back();
}
}