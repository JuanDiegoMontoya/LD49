#include "level.h"

// level definitions
namespace Game
{
  extern Level level1;
  extern Level level2;
  extern Level level3;
  extern Level level4;
  extern Level level5;
  extern Level level6;
  extern Level level7;
  extern Level level8;

  std::vector<Level*> levels
  {
    &level1,
    &level2,
    &level3,
    &level4,
    &level5,
    &level6,
    &level7,
    &level8,
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

    .customPlatforms =
    {
    },

    .winPlatformPos = { 25, 1, 0 },

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

    .customPlatforms =
    {
    },

    .winPlatformPos = { 0, 25, 0 },

    .winPlatformSize = { 15, 1, 15 },

    .startBombs = 5,

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

    .customPlatforms =
    {
    },

    .winPlatformPos = { 50, 1, 0 },

    .winPlatformSize = { 5, 1, 5 },

    .startBombs = 0,

    .startPos = { 0, 5, 0 },

    .nextLevel = &level4
  };

  Level level4
  {
    .name = "Skip and a Hop",

    .hint = "Jump continuously to maintain momentum",

    .bombs =
    {
    },

    .smallPlatforms =
    {
      { 20, 1, 0 },
    },

    .mediumPlatforms =
    {
      { 0, 1, 0 },
    },

    .largePlatforms =
    {
    },

    .customPlatforms =
    {
    },

    .winPlatformPos = { 33, 5, 0 },

    .winPlatformSize = { 1, 5, 5 },

    .startBombs = 1,

    .startPos = { 0, 5, 0 },

    .nextLevel = &level5
  };

  Level level5
  {
    .name = "Surface Tension",

    .hint = "Be very careful",

    .bombs =
    {
      { -8, 2.5, 5 },
      { -5, 2.5, 5 },
      { -2, 2.5, 5 },
      { 1, 2.5, 5 },
      { 4, 2.5, 5 },
      { 7, 2.5, 5 },

      { -8, 2.5, -5 },
      { -5, 2.5, -5 },
      { -2, 2.5, -5 },
      { 1, 2.5, -5 },
      { 4, 2.5, -5 },
      { 7, 2.5, -5 },

      { -8, 2.5, -2 },
      { -5, 2.5, -2 },
      { -2, 2.5, -2 },
      { 1, 2.5, -2 },
      { 4, 2.5, -2 },
      { 7, 2.5, -2 },

      { -8, 2.5, 2 },
      { -5, 2.5, 2 },
      { -2, 2.5, 2 },
      { 1, 2.5, 2 },
      { 4, 2.5, 2 },
      { 7, 2.5, 2 },
    },

    .smallPlatforms =
    {
    },

    .mediumPlatforms =
    {
    },

    .largePlatforms =
    {
      { 0, 1, 0 },
    },

    .customPlatforms =
    {
    },

    .winPlatformPos = { 80, 5, 0 },

    .winPlatformSize = { 2, 2, 2 },

    .startBombs = 0,

    .startPos = { 0, 5, 0 },

    .nextLevel = &level6
  };

  Level level6
  {
    .name = "Exploiter",

    .hint = "Place bombs below yourself while flying",

    .bombs =
    {
    },

    .smallPlatforms =
    {
      { 40, 11, 0 }
    },

    .mediumPlatforms =
    {
    },

    .largePlatforms =
    {
      { 0, 1, 0 },
    },

    .customPlatforms =
    {
    },

    .winPlatformPos = { 40, 10, 0 },

    .winPlatformSize = { 1, .51, 1 },

    .startBombs = 10,

    .startPos = { 0, 5, 0 },

    .nextLevel = &level7
  };

  Level level7
  {
    .name = "Blitzing Bomberman",

    .hint = "Try chaining bomb jumps",

    .bombs =
    {
      { 20, 5.5, 0 },
      { 35, 9.5, 0 },
    },

    .smallPlatforms =
    {
    },

    .mediumPlatforms =
    {
      { 0, 1, 0 },
    },

    .largePlatforms =
    {
    },

    .customPlatforms =
    {
      { { 0, 10, 0 }, { 1, 1, 1 } },
      { { 20, 4, 0 }, { .5, 1, .5 } },
      { { 35, 8, 0 }, { .5, 1, .5 } },
    },

    .winPlatformPos = { 50, 15, 0 },

    .winPlatformSize = { 2, 2, 2 },

    .startBombs = 1,

    .startPos = { 0, 5, 0 },

    .nextLevel = &level8
  };

  Level level8
  {
    .name = "Leap of Faith",

    .hint = "Good luck!",

    .bombs =
    {
      { 2, 3, 0 },
      { 0, 16, -13 },
      { 0, 16, -15 },
      { 0, 16, -17 },

      // monolith bombs
      { 28, 12, 5 },
      { 30, 12, 5 },
      { 32, 12, 5 },
      { 28, 12, 3 },
      { 30, 12, 3 },
      { 32, 12, 3 },
      { 28, 12, 7 },
      { 30, 12, 7 },
      { 32, 12, 7 },
      { 28, 14, 5 },
      { 30, 14, 5 },
      { 28, 14, 3 },
      { 30, 14, 3 },
      { 28, 14, 7 },
      { 30, 14, 7 },

      { 30, 23, -20 },
      { 45, 28, -20 },

      // back wall bombs
      { 63, 18, -20 },
      { 63, 20, -20 },
      { 63, 31, -15 },
      { 63, 33, -15 },
      { 65, 51, -20 }, // top of wall
    },

    .smallPlatforms =
    {
    },

    .mediumPlatforms =
    {
      { 0, 1, 0 },
      { 0, 15, -15 },
    },

    .largePlatforms =
    {
    },

    .customPlatforms =
    {
      // monolith
      { { 30, 40, 0 }, { 5, 40, 1 } },
      { { 30, 40, 10 }, { 5, 40, 1 } },
      { { 26, 40, 5 }, { 1, 40, 5 } },
      { { 34, 50, 5 }, { 1, 30, 5 } }, // back lip
      { { 30, 79, 5 }, { 3, 1, 4 } }, // ceiling
      { { 30, 10, 5 }, { 3, 1, 4 } }, // floor

      { { 30, 20, -20 }, { .5, 2, .5 } },
      { { 45, 25, -20 }, { .5, 2, .5 } },

      // back wall
      { { 65, 25, -20 }, { 1, 25, 20 } },
      { { 63, 15, -20 }, { .5, 2, .5 } },
      { { 63, 28, -15 }, { .5, 2, .5 } },
      { { 55, 35, -5 }, { 1, 1, 1 } },
    },

    .winPlatformPos = { 30, 78, 5 },

    .winPlatformSize = { 3, 1, 4 },

    .startBombs = 0,

    .startPos = { 0, 5, 0 },

    .nextLevel = nullptr
  };
}