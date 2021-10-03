#pragma once

#include <imgui.h>
#include "gfx/camera.h"
#include "game/game.h"

struct World
{
  bool paused = true;
  ImGuiIO* io{};
  GFX::Camera camera;
  Game::EntityManager entityManager;
};