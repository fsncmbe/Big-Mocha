#pragma once

#include <any>
#include <string>
#include <filesystem>
#include <typeindex>

#include <modules/ecs.hpp>

#define ASSET_PATH "assets/"
namespace mocha
{

enum class FileExtension
{
  ktxt,
  ksh,  // used for internal shader ending, as its composed of multiple files
  kvs,
  kfs,
  kobj,
  knull,
};

template<typename T>
struct ResourceHandle
{
  std::any data {};
  std::filesystem::path path {};
  int count {0};
  bool ready {false};
  std::filesystem::file_time_type lwt {};
};

struct Handle
{
  int uuid {0};
};

class Resource : public System
{
 public:
  Resource(std::filesystem::path path_to_assets);

  Handle load(std::filesystem::path path);

  template<typename T>
  T* get(int uuid);

  void unload(int uuid);

  void reload(int uuid);

  void update(float dt);

 private:
  std::filesystem::path asset_path;
  std::unordered_map<int, ResourceHandle<std::any>> resource_map {};
  int next_uuid {0};

  void checkMap(int uuid);
  void checkMap(std::filesystem::path path);
};

// Resource 
template<typename T>
T* Resource::get(int uuid)
{
  checkMap(uuid);
  return std::any_cast<T*>(&resource_map[uuid].data);
}

void Resource::checkMap(int uuid)
{
  if (resource_map.find(uuid) == resource_map.end())
    resource_map[uuid];
}


}