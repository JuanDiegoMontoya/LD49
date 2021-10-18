#pragma once

#include "macros.h"

struct Transform;
struct MeshHandle;
struct Renderable;

namespace GFX
{
  struct Camera;
  struct Mesh;

  class Renderer
  {
  public:
    Renderer();
    ~Renderer();

    NOCOPY_NOMOVE(Renderer)

    [[nodiscard]] MeshHandle GenerateMeshHandle(const Mesh& mesh);

    void Submit(const Transform& transform,
      const MeshHandle& mesh,
      const Renderable& renderable);
    void Draw(const Camera& camera, float dt);

  private:
    struct RendererImpl* impl_;
  };
}