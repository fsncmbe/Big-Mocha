#include <iostream>
#include <fstream>
#include <stack>
#include <unistd.h>
#include <chrono>

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

Resource::Resource(std::filesystem::path path)
{
  asset_path = std::filesystem::current_path();
  asset_path = asset_path / path;
}

Handle Resource::load(std::filesystem::path path)
{
  std::filesystem::path fpath = asset_path / path;
  std::any data = loadData(fpath);
  
  resource_map[next_uuid++] = ResourceHandle{.data{data}, .path{path},
        .count{1}, .ready{true}, .lwt{std::filesystem::last_write_time(fpath)}};

  return Handle{.uuid{next_uuid-1}};
}

std::any* Resource::get(int uuid)
{
  return &resource_map[uuid].data;
}

void Resource::unload(int uuid)
{
  resource_map.erase(uuid);
}

void Resource::reload(int uuid)
{
  auto path = asset_path / resource_map[uuid].path;

  resource_map[uuid].data = loadData(path);
  resource_map[uuid].lwt = std::filesystem::last_write_time(path);
}

void Resource::update(float dt)
{
  for (auto& [uuid, rh] : resource_map)
  {
    // Check if file has changed since last time
    auto ftime = std::filesystem::last_write_time(asset_path / rh.path);
    if (rh.lwt == ftime)
      continue;

    // Calculate the time in chorno time way
    auto chron_ftime = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
      ftime - std::filesystem::file_time_type::clock::now() + 
      std::chrono::system_clock::now()
    );
    // Get current time
    auto now = std::chrono::system_clock::now();

    // Hot reload after 1 sek
    if (now - chron_ftime >= std::chrono::seconds(1))
    {
      reload(uuid);
    }
  }
}

void Resource::checkMap(int uuid)
{
  if (resource_map.find(uuid) == resource_map.end())
    resource_map[uuid];
}


std::any loadData(std::filesystem::path path)
{
  namespace fsys = std::filesystem;

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
  {
    std::cout << "File couldnt open!\n";
  }
  
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