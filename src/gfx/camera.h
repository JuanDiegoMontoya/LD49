#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace GFX
{
  struct View
  {
    glm::vec3 position{};
    float pitch{}; // pitch angle in radians
    float yaw{};   // yaw angle in radians

    glm::vec3 GetForwardDir() const;
    glm::mat4 GetViewMatrix() const;
    void SetForwardDir(glm::vec3 dir);
  };

  struct Camera
  {
    View viewInfo{};
    glm::mat4 proj{};

    glm::mat4 GetViewProj() const { return proj * viewInfo.GetViewMatrix(); }
  };
}