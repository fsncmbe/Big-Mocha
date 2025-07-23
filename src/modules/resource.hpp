#pragma once

#include <any>
#include <string>

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

struct DataHandle
{
  std::any data;
  std::string path;
  int count {0};
  bool ready {false};
};

struct ResourceHandle
{
  int uuid;
};

/**
 * @brief Resource Singleton for all asset relates stuff
 * 
 */
class Resource : public System
{
 public:
  /**
   * @brief loads a file and returns its handle
   * 
   * @param path
   * @return ResourceHandle
   */
  ResourceHandle  load(const std::string& path);


  /**
   * @brief unloads a file, not possible for statics
   * 
   * @param uuid uuid of file
   */
  void            unload(int uuid);

  /**
   * @brief reloads a file, possible for statics
   * 
   * @param path 
   */
  void            reload(const std::string& path);

  /**
   * @brief reloads a file, possible for statics
   * 
   * @param uuid 
   */
  void            reload(int uuid);

  std::any*       get(int uuid);

  /**
   * @brief clears dynamic_map_ and sets data_count to size
   * 
   */
  void            clearDynamic();

  /**
   * @brief checks every frame if a dynamic files count <= 0 and deletes it
   * @todo  implement it all, with count updates and such 
   * @todo  also implement lookout for std::filesystem::last_write_time() to hot reload :)
   * @param dt
   */
  void            update(float dt);

 private:
  // file prefix decides if static = s_ or dynamic d_.
  // defaults to dynamic but use d_ for overview.
  int data_count_ {0};
  // static is all that is loaded before gameloop starts, cannot be changed after
  HandleMap<DataHandle> static_map_ {};
  // identifiers start at static_map.size() as static map does not change
  HandleMap<DataHandle> dynamic_map_ {};

  /**
   * @brief adds the data handle to either static or dynamic map
   * 
   * @param dynamic
   * @param handle
   * @return int = uuid
   */
  int add(bool dynamic, DataHandle handle);
};
}