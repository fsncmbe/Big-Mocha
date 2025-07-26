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
  std::any data;
  std::filesystem::path path;
  int count {0};
  bool ready {false};
  std::filesystem::file_time_type lwt;
};

template<typename T>
struct Handle
{
  int uuid;
};

template<typename T>
class ResourceMap
{
 public:
  Handle<T> load(std::filesystem::path path);

  T* get(int uuid);
  T* get(std::filesystem::path path);

  void unload(int uuid);
  void unload(std::filesystem::path path);

  void reload(int uuid);
  void reload(std::filesystem::path path);

  bool has(int uuid);
  bool has(std::filesystem::path path);

 private:
  std::vector<ResourceHandle<T>> handles;
  std::unordered_map<std::filesystem::path, int> path_to_int;
  int next_uuid {0};
};

class Resource : public System
{
 public:
  Resource(std::filesystem::path path_to_assets);

  template<typename T>
  Handle<T> load(std::filesystem::path path);

  template<typename T>
  T* get(int uuid);

  template<typename T>
  T* get(std::filesystem::path path);

  template<typename T>
  void unload(int uuid);

  template<typename T>
  void unload(std::filesystem::path path);

  template<typename T>
  void reload(int uuid);

  template<typename T>
  void reload(std::filesystem::path path);

  void update(float dt);

 private:
  std::filesystem::path asset_path;
  std::unordered_map<std::type_index, ResourceMap<std::any>> resource_map {};

  void checkMap(std::type_index t);
  void checkMap(std::type_index t, int uuid);
  void checkMap(std::type_index t, std::filesystem::path path);
};

// Use locally 
std::any loadData(std::filesystem::path path);

// ResourceMap

template<typename T>
Handle<T> ResourceMap<T>::load(std::filesystem::path path)
{
  
  T data = static_cast<T>(loadData(path));

  auto last_write_time =  std::filesystem::last_write_time(path);

  auto rh = ResourceHandle<T>{.data{data}, .path{path}, .count{1}, .ready{true}, .lwt{last_write_time}};

  handles.push_back(rh);

  return Handle<T>{next_uuid++};
}

template<typename T>
T* ResourceMap<T>::get(int uuid)
{
  return std::any_cast<T*>(&handles[uuid].data);
}

template<typename T>
T* ResourceMap<T>::get(std::filesystem::path path)
{
  return std::any_cast<T*>(&handles[path_to_int[path]].data);
}

template<typename T>
void ResourceMap<T>::unload(int uuid)
{
  
}

template<typename T>
void ResourceMap<T>::unload(std::filesystem::path path)
{
  
}

template<typename T>
void ResourceMap<T>::reload(int uuid)
{
  
}

template<typename T>
void ResourceMap<T>::reload(std::filesystem::path path)
{
  
}

template<typename T>
bool ResourceMap<T>::has(int uuid)
{
  for (ResourceHandle<T> rh : handles)
  {
    if (rh.)
  }
}

template<typename T>
bool ResourceMap<T>::has(std::filesystem::path path)
{
  
}

// Resource 

template<typename T>
Handle<T> Resource::load(std::filesystem::path path)
{
  auto t = std::type_index(typeid(T));
  checkMap(t);
  return resource_map[t].load(path);
}

template<typename T>
T* Resource::get(std::filesystem::path path)
{
  auto t = std::type_index(typeid(T));
  checkMap(t);
  return resource_map[t].get(path);
}

 template<typename T>
T* Resource::get(int uuid)
{
  auto t = std::type_index(typeid(T));
  checkMap(t);
  return resource_map[t].get(uuid);
}

template<typename T>
void Resource::unload(int uuid)
{
  auto t = std::type_index(typeid(T));
  checkMap(t);
  resource_map[t].unload(uuid);
}

template<typename T>
void Resource::unload(std::filesystem::path path)
{
  auto t = std::type_index(typeid(T));
  checkMap(t);
  resource_map[t].unload(path);
}

template<typename T>
void reload(int uuid)
{
  auto t = std::type_index(typeid(T));
  checkMap(t);
  resource_map[t].reload(uuid);
}

template<typename T>
void reload(std::filesystem::path path)
{
  auto t = std::type_index(typeid(T));
  checkMap(t);
  resource_map[t].reload(path);
}

void Resource::checkMap(std::type_index t)
{
  if (resource_map.find(t) == resource_map.end())
    resource_map[t];
}

void Resource::checkMap(std::type_index t, int uuid)
{
  checkMap(t);
  if (!resource_map[t].has(uuid))
    std::cout << "Id non existend\n";
}

void Resource::checkMap(std::type_index t, std::filesystem::path path)
{
  checkMap(t);
  if (!resource_map[t].has(path))
    std::cout << "Id non existend\n";
}


}