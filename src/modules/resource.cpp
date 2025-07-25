#include <iostream>
#include <fstream>

#include <modules/resource.hpp>
#include <modules/graphics.hpp>

namespace mocha
{

// Type specific loading Mechanisms
std::any loadData(std::filesystem::path path);
std::string loadFile(std::filesystem::path path);
Shader loadShader(std::filesystem::path path);
Model loadModel(std::filesystem::path path);
Textfile loadText(std::filesystem::path path);
FileExtension stringToExt(const std::string& ext);

DataHandle* Resource::getDataHandle(int uuid)
{
  if (uuid < static_map_.size())
  {
    return static_map_.get(uuid);
  }
  else
  {
    return &dynamic_map_[uuid];
  }
}

int Resource::add(bool dynamic, DataHandle handle)
{
  if (dynamic)
  {
    dynamic_map_[data_count_++] =  handle;
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
  fpath.append("assets/" + path);

  std::any data = loadData(fpath);

  auto last_write_time =  fsys::last_write_time(fpath);
  std::string fname = fpath.filename();
  bool dynamic = !(fname.substr(0, 2) == "s_");

  int uuid = add(dynamic, DataHandle{.data{data}, .path{fpath},
          .count{1}, .ready{true}, .last_write_time{last_write_time}});
  // IMPLEMENT THREAD LOADING HERE!!!
  return ResourceHandle{.uuid{uuid}};
}

void Resource::unload(int uuid)
{
  if (uuid < static_map_.size())
  {
    std::cout << "File is static!";
  }

  DataHandle* dh = &dynamic_map_[uuid];

  // differentiate between if there still is a reference or not
  if (dh->count > 0)
  {
    dh->ready = false;
    dh->data = nullptr;
  } else {
    dynamic_map_.erase(uuid);
  }
}

void Resource::reload(int uuid)
{
  DataHandle* dh = getDataHandle(uuid);
  std::any data = loadData(dh->path);
  auto last_write_time = std::filesystem::last_write_time(dh->path);

  static_map_.set(uuid, DataHandle{.data{data}, .path{dh->path}, .count{dh->count}, .ready{true}, .last_write_time{last_write_time}});
  // still not working
}

void Resource::replace(int uuid, const std::string& path)
{
  DataHandle* dh = getDataHandle(uuid);
  dh->data = loadData(path);
  dh->last_write_time = std::filesystem::last_write_time(dh->path);
}

std::any* Resource::get(int uuid)
{
  return &getDataHandle(uuid)->data;
}

void Resource::clearDynamic()
{
  data_count_ = static_map_.size();
  dynamic_map_.clear();
}

void Resource::update(float dt)
{
  for (auto& [uuid, data] : dynamic_map_)
  {
    DataHandle* dh = &data;
    if (dh->count <= 0)
    {
      dynamic_map_.erase(uuid);
      continue;
    }
    // hot reload
    auto current_lwt = std::filesystem::last_write_time(dh->path);
    if (dh->last_write_time.time_since_epoch() != current_lwt.time_since_epoch())
    {
      std::cout << "h";
      reload(uuid);
    }
  }
}

void Resource::decreaseCount(int uuid)
{
  dynamic_map_[uuid].count--;
}

void Resource::increaseCount(int uuid)
{
  dynamic_map_[uuid].count++;
}

std::any loadData(std::filesystem::path path)
{
  namespace fsys = std::filesystem;

  if (!fsys::exists(path))
    std::cout << "Missing file: " << path << "\n";

  std::any data;

  switch (stringToExt(path.extension()))
  {
    case (FileExtension::ksh):
    case (FileExtension::kvs):
    case (FileExtension::kfs):
      data = loadShader(path.string().substr(0, path.string().size()-3)); break;
    case (FileExtension::kobj):
      data = loadModel(path); break;
    case (FileExtension::ktxt):
      data = loadText(path); break;
  }
  
  return data;
}

std::string loadFile(std::filesystem::path path)
{
  std::ifstream file(path);

  if(!file.is_open())
    std::cout << "File couldnt open!\n";
  
  std::string out((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());
  file.close();
  return out;
}

Textfile loadText(std::filesystem::path path)
{
  return Textfile{.text{loadFile(path)}};
}

Shader loadShader(std::filesystem::path path)
{
  return Shader{};
}

Model loadModel(std::filesystem::path path)
{
  return Model{};
}

FileExtension stringToExt(const std::string& ext)
{
  if (ext == ".txt")  return FileExtension::ktxt;
  if (ext == ".sh")   return FileExtension::ksh;
  if (ext == ".vs")   return FileExtension::kvs;
  if (ext == ".fs")   return FileExtension::kfs;
  if (ext == ".obj")  return FileExtension::kobj;
  return FileExtension::knull;
}

}