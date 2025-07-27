#pragma once

#include <any>
#include <string>
#include <filesystem>
#include <typeindex>

#include <modules/ecs.hpp>

#define ASSET_PATH "assets/"

#define CAST_TEXTFILE(o) std::any_cast<mocha::Textfile>(o)


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

struct ResourceHandle
{
  std::any data {};
  std::filesystem::path path {};
  int count {0};
  bool ready {false};
  std::filesystem::file_time_type lwt {};
  ResourceHandle copy()
  {
    return ResourceHandle{.path{path}, .count{count}};
  }
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

  std::any* get(int uuid);

  void unload(int uuid);

  void reload(int uuid);

  void update(float dt);

 private:
  std::filesystem::path asset_path;
  std::unordered_map<int, ResourceHandle> resource_map {};
  int next_uuid {0};

  void checkMap(int uuid);
};

}