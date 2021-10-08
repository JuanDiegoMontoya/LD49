#include "level.h"

// level definitions
namespace Game
{
  extern Level level_explosive_entrance;
  extern Level level_high_jump;
  extern Level level_short_supply;
  extern Level level_rock_climb;
  extern Level level_claustrophobia;
  extern Level level_skip_hop;
  extern Level level_surface_tension;
  extern Level level_exploiter;
  extern Level level_blitz;
  extern Level level_leap_of_faith;

  std::vector<Level*> levels
  {
    &level_explosive_entrance,
    &level_high_jump,
    &level_short_supply,
    &level_rock_climb,
    &level_claustrophobia,
    &level_skip_hop,
    &level_surface_tension,
    &level_exploiter,
    &level_blitz,
    &level_leap_of_faith,
  };

  Level level_explosive_entrance
  {
    .name = "An Explosive Entrance",

    .hint = "Drop or jump onto bombs to detonate them",

    .bombs =
    {
      { 5, 3, 0 },
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

    .startBombs = 4,

    .startPos = { 0, 5, 0 },

    .nextLevel = &level_high_jump
  };

  Level level_high_jump
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

    .nextLevel = &level_short_supply
  };

  Level level_short_supply
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

    .nextLevel = &level_claustrophobia
  };

  Level level_claustrophobia
  {
    .name = "Claustrophobia",

    .hint = "Jump and put the bomb under youself",

    .bombs =
    {
    },

    .smallPlatforms =
    {
      { 0, 10, 0 },
    },

    .mediumPlatforms =
    {
    },

    .largePlatforms =
    {
    },

    .customPlatforms =
    {
      { { 2, 15, 0 }, { 1, 6, 3 } },
      { { -2, 15, 0 }, { 1, 6, 3 } },

      { { 0, 15, 2 }, { 3, 6, 1 } },
      { { 0, 15, -2 }, { 3, 6, 1 } },
    },

    .winPlatformPos = { 0, 1, 0 },

    .winPlatformSize = { 8, 1, 8 },

    .startBombs = 1,

    .startPos = { 0, 15, 0 },

    .nextLevel = &level_skip_hop
  };

  Level level_skip_hop
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

    .nextLevel = &level_rock_climb
  };

  Level level_rock_climb
  {
    .name = "Rock Climb",

    .hint = "Scale the cliff!",

    .bombs =
    {
      { 3, 2.5, -1 },
      { 3, 2.5, 1 },

      { 10, 21, -5 },
      { 10, 21, 1 },

      { 12, 41, 5 },

      { 14, 51, 0 },
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
      { { 10, 10, 0 }, { 1, 10, 10 } },
      { { 12, 30, 0 }, { 1, 10, 10 } },
      { { 14, 45, 0 }, { 1, 5, 10 } },
    },

    .winPlatformPos = { 40, 10, 0 },

    .winPlatformSize = { 3, 1, 3 },

    .startBombs = 0,

    .startPos = { 0, 5, 0 },

    .nextLevel = &level_surface_tension
  };

  Level level_surface_tension
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

    .winPlatformSize = { 3, 3, 3 },

    .startBombs = 0,

    .startPos = { 0, 5, 0 },

    .nextLevel = &level_exploiter
  };

  Level level_exploiter
  {
    .name = "Exploiter",

    .hint = "Place bombs below yourself while flying",

    .bombs =
    {
    },

    .smallPlatforms =
    {
    },

    .mediumPlatforms =
    {
      { 40, 11, 0 }
    },

    .largePlatforms =
    {
      { 0, 1, 0 },
    },

    .customPlatforms =
    {
    },

    .winPlatformPos = { 40, 10, 0 },

    .winPlatformSize = { 2, .51, 2 },

    .startBombs = 10,

    .startPos = { 0, 5, 0 },

    .nextLevel = &level_blitz
  };

  Level level_blitz
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

    .nextLevel = &level_leap_of_faith
  };

  Level level_leap_of_faith
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