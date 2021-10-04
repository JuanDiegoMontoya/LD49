#include "level.h"

// level definitions
namespace Game
{
  Level level1
  {
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

    .nextLevel = nullptr
  };
}