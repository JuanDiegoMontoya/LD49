#pragma once

#include <vector>

#include <glm/vec3.hpp>

namespace Game
{
  struct Level
  {
    // layout
    std::vector<glm::vec3> bombs;
    std::vector<glm::vec3> smallPlatforms;
    std::vector<glm::vec3> mediumPlatforms;
    std::vector<glm::vec3> largePlatforms;

    glm::vec3 winPlatformPos{};
    glm::vec3 winPlatformSize{};

    int startBombs{ 0 };
    glm::vec3 startPos{};

    Level* nextLevel{ nullptr };
  };

  extern Level level1;
  //extern Level level2;
}