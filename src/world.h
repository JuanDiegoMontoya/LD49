#pragma once

#include <imgui.h>
#include "gfx/camera.h"
#include "game/game.h"
#include "game/physics.h"

constexpr glm::vec4 EXPLOSIVE_COLOR{ .8, .4, .1, 1.0 };
constexpr float EXPLOSIVE_SIZE = 0.7f;
constexpr glm::vec3 SELECT_GLOW{ 0.3f };
constexpr float PLAYER_HEIGHT = 2.0f;
constexpr float PLAYER_RADIUS = 0.7f;
constexpr float SELECT_DISTANCE = 3.0f;
constexpr glm::vec3 PLACEMENT_VALID{ .1, .5, .1 };
constexpr glm::vec3 PLACEMENT_INVALID{ .5, .1, .1 };
constexpr float EXPLOSIVE_TRIGGER_FORCE = 15000.0;

struct World
{
  bool cheats = false;
  bool paused = true;
  int bombInventory = 1;

  ImGuiIO* io{};
  GFX::Camera camera;
  Game::EntityManager entityManager;

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
};