#pragma once

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

namespace GFX
{
  struct Camera;
  struct Mesh;

  struct MeshHandle
  {
    uint32_t count{};
    uint32_t vertexBuffer{};
    uint32_t indexBuffer{};
  };

  struct Renderable
  {
    glm::vec3 position{};
    glm::vec3 scale{ 1 };
    glm::quat rotation{ 1, 0, 0, 0 };

    MeshHandle handle{};
  };

  class Renderer
  {
  public:
    Renderer();
    ~Renderer();

    MeshHandle GenerateMeshHandle(const Mesh& mesh);

    void Submit(const Renderable& renderable);
    void Draw(const Camera& camera);

  private:
    struct RendererImpl* impl_;
  };
}