#pragma once

#include <vector>
#include <string_view>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace GFX
{
  struct Vertex
  {
    glm::vec3 position{};
    glm::vec3 normal{};
    glm::vec2 texcoord{};
  };

  std::vector<Vertex> LoadMesh(std::string_view file);
}