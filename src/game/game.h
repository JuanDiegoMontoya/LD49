#pragma once

#include <vector>

#include "macros.h"
#include "components.h"
#include "gfx/renderer.h"

namespace Game
{
  using entity_t = int32_t;
  constexpr entity_t null_entity = 0;

  struct GameObject
  {
    entity_t entity{};

    EntityType type;
    Transform transform;
    MeshHandle mesh;
    Renderable renderable;
    PhysicsFlags physics{};
  };

  class EntityManager
  {
  public:
    EntityManager() {};
    ~EntityManager() {};

    NOCOPY_NOMOVE(EntityManager)

    entity_t CreateEntity();
    GameObject* GetObject(entity_t entity);
    auto& GetObjects() { return objects; }
    void DestroyEntity(entity_t entity);
    void Clear();

  private:
    entity_t nextEntity = 0;

    std::vector<GameObject*> objects;
  };
}