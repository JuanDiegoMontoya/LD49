#pragma once

#include <cstdint>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/quaternion.hpp>

enum class EntityType
{
  TERRAIN,
  EXPLOSIVE,
};

struct Transform
{
  glm::vec3 position{};
  glm::vec3 scale{ 1 };
  glm::quat rotation{ 1, 0, 0, 0 };

  glm::mat4 GetModel() const;
};

struct MeshHandle
{
  uint32_t count{};
  uint32_t vertexBuffer{};
  uint32_t indexBuffer{};
};

struct Renderable
{
  // flags or sum'
  bool visible : 1 = false;
};

//enum class PhysicsType
//{
//
//};

struct PhysicsFlags
{
  
};