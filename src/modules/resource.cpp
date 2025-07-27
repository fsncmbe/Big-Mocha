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
  const char* data_stream = (loadFile(path) + '\n').c_str();

  std::vector<Vertex>       vertices;
  std::vector<unsigned int> indices;
  std::vector<glm::vec3>    t_pos;
  std::vector<glm::vec3>    t_normal;
  std::vector<glm::vec2>    t_uvs;

  const char* line_start = data_stream;

  for (const char* c = line_start; *c != '\0'; ++c)
  {
    if (*c == '\n')
    {
      std::string line(line_start, c);

      std::istringstream line_stream(line);
      std::string t;

      line_stream >> t;

      if (t == "v")
      {
        glm::vec3 pos;
        line_stream >> pos.x >> pos.y >> pos.z;
        t_pos.push_back(pos);
      }
      else if (t == "vn")
      {
        glm::vec3 normal;
        line_stream >> normal.x >> normal.y >> normal.z;
        t_normal.push_back(normal);
      }
      else if (t == "vt")
      {
        glm::vec2 uv;
        line_stream >> uv.x >> uv.y;
        t_uvs.push_back(uv);
      }
      else if (t == "f")
      {
        const int f_nums = 3;
        std::string strings[f_nums];
        for (int i=0; i<f_nums; i++)
        {
          line_stream >> strings[i];
          unsigned int v_index, uv_index, n_index;
          sscanf(strings[i].c_str(), "%d/%d/%d", v_index, uv_index, n_index);

          Vertex v;

          v.position = t_pos[v_index-1];
          v.tex_pos = t_uvs[uv_index-1];
          v.normal = t_normal[n_index-1];

          auto it = std::find(vertices.begin(), vertices.end(), v);

          if (it != vertices.end())
          {
            int index = std::distance(vertices.begin(), it);
            indices.push_back(index);
          }
          else
          {
            vertices.push_back(v);
            indices.push_back(vertices.size()-1);
          }
        }
      }
      line_start = c + 1;
    }
  }

  unsigned int vao, vbo, ebo;

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
  
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_pos));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

  glBindVertexArray(0);

  return Model{.indices_count{(int)indices.size()}, .vao{vao}};
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