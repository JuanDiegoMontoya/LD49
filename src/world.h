#pragma once

#include <imgui.h>
#include "gfx/camera.h"
#include "game/game.h"
#include "game/physics.h"

struct World
{
  bool cheats = false;
  bool paused = true;
  ImGuiIO* io{};
  GFX::Camera camera;
  Game::EntityManager entityManager;

  MeshHandle sphereMeshHandle;
  MeshHandle cubeMeshHandle;

  Game::GameObject* MakeExplosive(glm::vec3 pos, float scale, Game::Physics* physics)
  {
    Game::GameObject* obj = entityManager.GetObject(entityManager.CreateEntity());
    obj->transform.position = pos;
    obj->transform.scale = glm::vec3(scale);
    obj->mesh = sphereMeshHandle;
    obj->renderable.visible = true;
    obj->type = EntityType::EXPLOSIVE;

    Game::Sphere sphere{ scale };
    physics->AddObject(obj, Game::MaterialType::OBJECT, &sphere);
    return obj;
  }
};