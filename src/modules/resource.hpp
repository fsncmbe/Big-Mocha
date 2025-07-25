#pragma once

#include <any>
#include <string>
#include <filesystem>

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
  std::any* data;
  std::filesystem::path path;
  int count {0};
  bool ready {false};
  std::filesystem::file_time_type last_write_time;
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
   * @param uuid 
   */
  void            reload(int uuid);

  /**
   * @brief replaces data of a uuid with file from another path
   * 
   * @param uuid of file
   * @param path of file
   */
  void            replace(int uuid, const std::string& path);

  /**
   * @brief Get Data from given resource handle uuid
   * 
   * @param uuid 
   * @return std::any* is type of data saved, not DataHandle!
   */
  std::any*       get(int uuid);

  /**
   * @brief clears dynamic_map_ and sets data_count to size
   * 
   */
  void            clearDynamic();

  /**
   * @brief checks every frame if a dynamic files count <= 0 and deletes it
   * 
   * @param dt
   */
  void            update(float dt);
  
  /**
   * @brief decreases count of DataHandle with uuid
   * 
   * @param uuid 
   */
  void            decreaseCount(int uuid);

  /**
   * @brief increases count of DataHandle with uuid
   * 
   * @param uuid 
   */
  void            increaseCount(int uuid);

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

  /**
   * @brief Get the Data Handle of uuid, change of 
   * 
   * @param uuid 
   * @return DataHandle* 
   */
  DataHandle*     getDataHandle(int uuid);
};
}