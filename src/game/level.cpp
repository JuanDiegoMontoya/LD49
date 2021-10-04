#include "level.h"

// level definitions
namespace Game
{
  extern Level level1;
  extern Level level2;
  extern Level level3;

  std::vector<Level*> levels
  {
    &level1,
    &level2,
    &level3,
  };

  Level level1
  {
    .name = "An Explosive Entrance",

    .hint = "Drop bombs from high to detonate them",

    .bombs =
    {
    },

    .smallPlatforms =
    {
    },

    .mediumPlatforms =
    {
    },

    .largePlatforms =
    {
      { 0, 1, 0 }
    },

    .winPlatformPos = { 30, 1, 0 },

    .winPlatformSize = { 5, 1, 10 },

    .startBombs = 99,

    .startPos = { 0, 5, 0 },

    .nextLevel = &level2
  };

  Level level2
  {
    .name = "High Jump",

    .hint = "Look up, and use multiple bombs",

    .bombs =
    {
    },

    .smallPlatforms =
    {
    },

    .mediumPlatforms =
    {
    },

    .largePlatforms =
    {
      { 0, 1, 0 }
    },

    .winPlatformPos = { 0, 25, 0 },

    .winPlatformSize = { 15, 1, 15 },

    .startBombs = 3,

    .startPos = { 0, 5, 0 },

    .nextLevel = &level3
  };

  Level level3
  {
    .name = "Short Supply",

    .hint = "Pick up bombs",

    .bombs =
    {
      { 3, 2.5, 0 },
      { 25, 2.5, 0 },
    },

    .smallPlatforms =
    {
    },

    .mediumPlatforms =
    {
      { 0, 1, 0 },
      { 25, 1, 0 },
    },

    .largePlatforms =
    {
    },

    .winPlatformPos = { 50, 1, 0 },

    .winPlatformSize = { 5, 1, 5 },

    .startBombs = 0,

    .startPos = { 0, 5, 0 },

    .nextLevel = nullptr
  };
}