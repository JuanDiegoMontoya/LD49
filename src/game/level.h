#pragma once

#include <vector>
#include <utility>

#include <glm/vec3.hpp>

namespace Game
{
  struct Level
  {
    const char* name{};
    const char* hint{};

    // layout
    std::vector<glm::vec3> bombs;
    std::vector<glm::vec3> smallPlatforms;
    std::vector<glm::vec3> mediumPlatforms;
    std::vector<glm::vec3> largePlatforms;
    std::vector<std::pair<glm::vec3, glm::vec3>> customPlatforms;

    glm::vec3 winPlatformPos{};
    glm::vec3 winPlatformSize{};

    int startBombs{ 0 };
    glm::vec3 startPos{};

    Level* nextLevel{ nullptr };
  };

  extern std::vector<Level*> levels;
}