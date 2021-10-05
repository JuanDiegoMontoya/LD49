#pragma once

#include <imgui.h>
#include "gfx/camera.h"
#include "game/game.h"
#include "game/physics.h"
#include "game/level.h"

#define DEBUG_PRINT 0
#if DEBUG_PRINT
#define DEBUG_PRINT(x) printf(#x "\n");
#else
#define DEBUG_PRINT(x)
#endif

constexpr glm::vec4 EXPLOSIVE_COLOR{ 1.0, .3, .1, 1.0 };
constexpr glm::vec3 EXPLOSIVE_BASE_GLOW{ .2, 0, 0 };
constexpr float EXPLOSIVE_TRIGGER_FORCE = 15000.0;
constexpr float EXPLOSIVE_SIZE = 0.7f;
constexpr float EXPLOSION_RECURSE_DIST = 7.0;
constexpr float EXPLOSION_MAX_PLAYER_DIST = 8.0;
constexpr float EXPLOSION_MAX_OBJECT_DIST = 10.0;
constexpr float EXPLOSION_PLAYER_FORCE = 20.0;
constexpr float EXPLOSION_MIN_PLAYER_FORCE = 10.0;
constexpr float EXPLOSION_OBJECT_FORCE = 30.0;
constexpr float EXPLOSION_PLAYER_TRIGGER_FORCE = 10.0;

constexpr float PLAYER_HEIGHT = 2.0f;
constexpr float PLAYER_RADIUS = 0.7f;

constexpr glm::vec3 SELECT_GLOW{ 0.3f };
constexpr float SELECT_DISTANCE = 3.0f;
constexpr glm::vec3 PLACEMENT_VALID{ .1, .5, .1 };
constexpr glm::vec3 PLACEMENT_INVALID{ .5, .1, .1 };

constexpr glm::vec3 SMALL_PLATFORM_SIZE{ 2, 1, 2 };
constexpr glm::vec3 MEDIUM_PLATFORM_SIZE{ 5, 1, 5 };
constexpr glm::vec3 LARGE_PLATFORM_SIZE{ 10, 1, 10 };

constexpr int POCKET_SIZE = 5;

enum class GameState
{
  PAUSED,
  UNPAUSED,
  DEAD,
  WIN_LEVEL,
  WIN_GAME,
};

struct World
{
  float mouseSensitivity = 0.003f;
  bool cheats = false;
  GameState gameState = GameState::PAUSED;
  int bombInventory = 1;
  unsigned deathCounter = 0;

  ImGuiIO* io{};
  GFX::Camera camera;
  Game::EntityManager entityManager;

  const Game::Level* currentLevel = nullptr;

  MeshHandle sphereMeshHandle;
  MeshHandle cubeMeshHandle;

  Game::GameObject* MakeSphere(glm::vec3 pos, float scale)
  {
    Game::GameObject* obj = entityManager.GetObject(entityManager.CreateEntity());
    obj->transform.position = pos;
    obj->transform.scale = glm::vec3(scale);
    obj->mesh = sphereMeshHandle;
    obj->renderable.visible = true;
    return obj;
  }

  Game::GameObject* MakeBox(glm::vec3 pos, glm::vec3 halfExtents)
  {
    Game::GameObject* obj = entityManager.GetObject(entityManager.CreateEntity());
    obj->transform.position = pos;
    obj->transform.scale = glm::vec3(halfExtents);
    obj->mesh = cubeMeshHandle;
    obj->renderable.visible = true;
    return obj;
  }

  Game::GameObject* MakeExplosive(glm::vec3 pos, Game::Physics* physics)
  {
    Game::GameObject* obj = MakeBox(pos, glm::vec3(EXPLOSIVE_SIZE));
    obj->renderable.color = EXPLOSIVE_COLOR;
    obj->type = EntityType::EXPLOSIVE;
    Game::Box box{ glm::vec3(EXPLOSIVE_SIZE) };
    physics->AddObject(obj, Game::MaterialType::OBJECT, &box);
    return obj;
  }

  Game::GameObject* MakePlatform(glm::vec3 pos, glm::vec3 halfExtents, Game::Physics* physics)
  {
    auto* obj = MakeBox(pos, halfExtents);
    obj->type = EntityType::TERRAIN;
    Game::Box box{ halfExtents };
    physics->AddObject(obj, Game::MaterialType::TERRAIN, &box);
    return obj;
  }

  void LoadLevel(const Game::Level& level, Game::Physics* physics)
  {
    gameState = GameState::PAUSED;
    currentLevel = &level;
    camera.viewInfo.pitch = 0;
    camera.viewInfo.yaw = 0;
    entityManager.Clear();
    physics->Reset();

    for (glm::vec3 pos : level.bombs)
    {
      MakeExplosive(pos, physics);
    }

    for (glm::vec3 pos : level.smallPlatforms)
    {
      MakePlatform(pos, SMALL_PLATFORM_SIZE, physics);
    }

    for (glm::vec3 pos : level.mediumPlatforms)
    {
      MakePlatform(pos, MEDIUM_PLATFORM_SIZE, physics);
    }

    for (glm::vec3 pos : level.largePlatforms)
    {
      MakePlatform(pos, LARGE_PLATFORM_SIZE, physics);
    }

    for (auto& [pos, size] : level.customPlatforms)
    {
      MakePlatform(pos, size, physics);
    }

    auto* win = MakePlatform(level.winPlatformPos, level.winPlatformSize, physics);
    win->physics.isWinPlatform = true;
    win->renderable.glow = { 0, .4, .9 };
    win->renderable.color = { .05, .05, .05, 1.0 };

    bombInventory = level.startBombs;

    physics->SetPlayerPos(level.startPos);

    // epic hack
    physics->Simulate(0);

    DEBUG_PRINT(LoadLevela);
  }
};