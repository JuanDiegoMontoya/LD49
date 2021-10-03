#pragma once

#include "macros.h"

#include <glm/vec3.hpp>

struct PhysicsImpl;
struct Transform;

struct World;

namespace GFX
{
  struct View;
  struct Mesh;
  struct Camera;
}

struct ImGuiIO;

namespace Game
{
  struct GameObject;

  enum class MaterialType
  {
    PLAYER,  // character controller
    TERRAIN, // static
    OBJECT,  // dynamic
  };

  struct Shape { virtual int id() { return 0; }; };
  struct Sphere : Shape { Sphere(float r) : radius(r) {} float radius{}; };
  struct Box : Shape { Box(glm::vec3 he) : halfExtents(he) {} glm::vec3 halfExtents; };

  using collider_t = void*;

  class Physics
  {
  public:
    Physics();
    ~Physics();

    NOCOPY_NOMOVE(Physics)

    //collider_t CookMesh(const GFX::Mesh& mesh);
    void SetWorld(World* world);
    void Simulate(float dt);

    //void AddObject(GameObject* object, MaterialType material, collider_t mesh);
    void AddObject(GameObject* object, MaterialType material, const Shape* shape);
    void RemoveObject(GameObject* object);

    void SetObjectTransform(GameObject* object, Transform transform);

  private:
    PhysicsImpl* impl_{};
  };
}