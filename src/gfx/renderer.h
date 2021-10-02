#pragma once

#include "macros.h"
#include "components.h"

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

    MeshHandle GenerateMeshHandle(const Mesh& mesh);

    void Submit(const Transform& transform,
      const MeshHandle& mesh,
      const Renderable& renderable);
    void Draw(const Camera& camera);

  private:
    struct RendererImpl* impl_;
  };
}