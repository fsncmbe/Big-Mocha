#include <iostream>
#include <filesystem>
#include <fstream>

#include <modules/resource.hpp>
#include <modules/graphics.hpp>

namespace mocha
{

  // Type specific loading Mechanisms
  std::string loadFile(const std::string& path);
  Shader loadShader(const std::string& path);
  Model loadModel(const std::string& path);

  FileExtension stringToExt(const std::string& ext)
  {
    if (ext == ".txt")  return FileExtension::ktxt;
    if (ext == ".sh")   return FileExtension::ksh;
    if (ext == ".vs")   return FileExtension::kvs;
    if (ext == ".fs")   return FileExtension::kfs;
    if (ext == ".obj")  return FileExtension::kobj;
    return FileExtension::knull;
  }

  int Resource::add(bool dynamic, DataHandle handle)
  {
    if (dynamic)
    {
      dynamic_map_.add(data_count_++, handle);
    } else
    {
      static_map_.add(data_count_++, handle);
    }

    return data_count_ - 1;
  }

  ResourceHandle Resource::load(const std::string& path)
  {
    namespace fsys = std::filesystem;

    fsys::path fpath = fsys::current_path();
    fpath.append(ASSET_PATH);
    fpath.append(path);

    if (!fsys::exists(fpath))
      std::cout << "Missing file: " << fpath << "\n";
    
    DataHandle d_handle = {.path{path}};

    switch (stringToExt(fpath.extension()))
    {
      case (FileExtension::ksh):
      case (FileExtension::kvs):
      case (FileExtension::kfs):
        d_handle.data = loadShader(path.substr(0, path.size()-3)); break;
      case (FileExtension::kobj):
        d_handle.data = loadModel(path); break;
      case (FileExtension::ktxt):
        d_handle.data = loadFile(path); break;
    }

    std::string fname = fpath.filename();
    bool dynamic = !(fname.substr(0, 2) == "s_");

    int uuid = add(dynamic, d_handle);

    return ResourceHandle{.uuid{uuid}};
  }

  std::any* Resource::get(int uuid)
  {
    if (uuid < static_map_.size())
    {
      return &static_map_.get(uuid)->data;
    }
    else
    {
      return &dynamic_map_.get(uuid)->data;
    }
  }

  void Resource::clearDynamic()
  {
    data_count_ = static_map_.size();
    dynamic_map_.clear();
  }

  void Resource::update(float dt)
  {
    
  }

  std::string loadFile(const std::string& path)
  {
    std::string fpath = ASSET_PATH + path;
    std::ifstream file(fpath);

    if(!file.is_open())
      std::cout << "File couldnt open!\n";
    
    std::string out((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
    file.close();
    return out;
  }

  Shader loadShader(const std::string& path)
  {
    return Shader{};
  }

  Model loadModel(const std::string& path)
  {
    return Model{};
  }

}