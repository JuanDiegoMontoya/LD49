#include "game.h"

namespace Game
{
  entity_t EntityManager::CreateEntity()
  {
    GameObject obj{};
    obj.entity = ++nextEntity;
    obj.type = EntityType::REGULAR;
    objects.push_back(obj);
    return nextEntity;
  }

  GameObject& EntityManager::GetObject(entity_t entity)
  {
    for (size_t i = 0; i < objects.size(); i++)
    {
      if (objects[i].entity == entity)
      {
        return objects[i];
      }
    }

    assert(0 && "Tried to retrieve object that didn't exist!");
    return objects.front();
  }

  void EntityManager::DestroyEntity(entity_t entity)
  {
    for (size_t i = 0; i < objects.size(); i++)
    {
      if (objects[i].entity == entity)
      {
        //printf("Destroyed object %d\n", objects[i]->entity);
        objects[i] = std::move(objects.back());
        objects.pop_back();
        return;
      }
    }

    assert(0 && "Tried to delete an object that didn't exist!");
  }

  void EntityManager::Clear()
  {
    objects.clear();
    nextEntity = 0;
  }
}