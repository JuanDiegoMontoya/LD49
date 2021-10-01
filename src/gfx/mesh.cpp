#include "mesh.h"

#include <stdexcept>
#include <iostream>

#include <tiny_obj_loader.h>

namespace GFX
{
  Mesh LoadMesh(std::string_view file)
  {
    Mesh mesh;

    tinyobj::ObjReaderConfig reader_config;
    reader_config.triangulate = true;

    tinyobj::ObjReader reader;

    std::string actualFile = "assets/models/" + std::string(file);
    if (!reader.ParseFromFile(actualFile, reader_config))
    {
      if (!reader.Error().empty())
      {
        throw std::runtime_error(reader.Error());
      }
      exit(1);
    }

    if (!reader.Warning().empty())
    {
      std::cerr << "TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++)
    {
      // Loop over faces(polygon)
      size_t index_offset = 0;
      for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
      {
        size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

        // Loop over vertices in the face.
        for (size_t v = 0; v < fv; v++)
        {
          // access to vertex
          tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
          
          Vertex vertex{};

          vertex.position.x = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
          vertex.position.y = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
          vertex.position.z = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

          // Check if `normal_index` is zero or positive. negative = no normal data
          if (idx.normal_index >= 0)
          {
            vertex.normal.x = attrib.normals[3 * size_t(idx.normal_index) + 0];
            vertex.normal.y = attrib.normals[3 * size_t(idx.normal_index) + 1];
            vertex.normal.z = attrib.normals[3 * size_t(idx.normal_index) + 2];
          }

          // Check if `texcoord_index` is zero or positive. negative = no texcoord data
          if (idx.texcoord_index >= 0)
          {
            vertex.texcoord.x = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
            vertex.texcoord.y = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
          }

          mesh.vertices.push_back(vertex);
        }
        index_offset += fv;

        // per-face material
        shapes[s].mesh.material_ids[f];
      }
    }

    return mesh;
  }
}