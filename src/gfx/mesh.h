#pragma once

#include <vector>
#include <string_view>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace GFX
{
  using index_t = uint32_t;

  struct Vertex
  {
    glm::vec3 position{};
    glm::vec3 normal{};
    glm::vec2 texcoord{};
  };

  struct Mesh
  {
    std::vector<Vertex> vertices;
  };


  Mesh LoadMesh(std::string_view file);
}