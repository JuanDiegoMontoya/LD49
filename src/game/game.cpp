#include "game.h"

namespace Game
{
  entity_t EntityManager::CreateEntity()
  {
    GameObject* obj = new GameObject;
    obj->entity = ++nextEntity;
    obj->type = EntityType::REGULAR;
    objects.push_back(obj);
    return nextEntity;
  }

  GameObject* EntityManager::GetObject(entity_t entity)
  {
    for (size_t i = 0; i < objects.size(); i++)
    {
      if (objects[i]->entity == entity)
      {
        return objects[i];
      }
    }

    return nullptr;
  }

  void EntityManager::DestroyEntity(entity_t entity)
  {
    for (size_t i = 0; i < objects.size(); i++)
    {
      if (objects[i]->entity == entity)
      {
        //printf("Destroyed object %d\n", objects[i]->entity);
        delete objects[i];
        objects.erase(objects.begin() + i);
        return;
      }
    }

    assert(0 && "Tried to delete an object that didn't exist!");
  }

  void EntityManager::Clear()
  {
    for (auto* object : objects)
    {
      delete object;
    }

    objects.clear();
    nextEntity = 0;
  }
}