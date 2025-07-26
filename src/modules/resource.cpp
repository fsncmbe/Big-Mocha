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

Resource::Resource(std::filesystem::path path)
{
  asset_path = std::filesystem::current_path();
  asset_path += "/" + path.string();
}

Handle Resource::load(std::filesystem::path path)
{
   std::any data = loadData(path);
   resource_map[next_uuid++] = ResourceHandle<std::any>{.data{data}, .path{path}, 
        .count{1}, .ready{true}, .lwt{std::filesystem::last_write_time(path)}};

  return Handle{.uuid{next_uuid-1}};
}

v

void Resource::update(float dt)
{
  
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