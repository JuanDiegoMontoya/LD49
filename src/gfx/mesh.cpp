#include "mesh.h"

#include <stdexcept>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <tuple>
#include <functional>

#include <tiny_obj_loader.h>


template<typename T> struct hash;

template<>
struct hash<GFX::Vertex>
{
  std::size_t operator()(const GFX::Vertex& a) const noexcept;
};

namespace
{
  template <class T>
  inline void hash_combine(std::size_t& seed, T const& v)
  {
    seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

  // Recursive template code derived from Matthieu M.
  template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
  struct HashValueImpl
  {
    static void apply(size_t& seed, Tuple const& tuple)
    {
      HashValueImpl<Tuple, Index - 1>::apply(seed, tuple);
      hash_combine(seed, std::get<Index>(tuple));
    }
  };

  template <class Tuple>
  struct HashValueImpl<Tuple, 0>
  {
    static void apply(size_t& seed, Tuple const& tuple)
    {
      hash_combine(seed, std::get<0>(tuple));
    }
  };

  template <typename ... TT>
  struct hash<std::tuple<TT...>>
  {
    size_t operator()(std::tuple<TT...> const& tt) const
    {
      size_t seed = 0;
      HashValueImpl<std::tuple<TT...> >::apply(seed, tt);
      return seed;
    }
  };
}


std::size_t hash<GFX::Vertex>::operator()(const GFX::Vertex& a) const noexcept
{
  auto tup = std::make_tuple(a.position.x, a.position.y, a.position.z, a.normal.x, a.normal.y, a.normal.z);
  hash<decltype(tup)> hasher;
  return hasher(tup);
}


namespace GFX
{
  Mesh LoadMesh(std::string_view file)
  {
    std::vector<Vertex> vertices;

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
      throw std::runtime_error("Failed to parse mesh!");
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

          vertices.push_back(vertex);
        }
        index_offset += fv;

        // per-face material
        shapes[s].mesh.material_ids[f];
      }
    }



    Mesh mesh;
    
    mesh.vertices.resize(vertices.size());
    mesh.indices.resize(vertices.size());
    for (size_t i = 0; i < vertices.size(); i++)
    {
      mesh.vertices[i] = vertices[i];
      mesh.indices[i] = i;
    }

    // TODO: fix this algorithm
    //std::unordered_map<Vertex, index_t, hash<Vertex>> vertexToIndex;
    //index_t curIndex = 0;

    //for (const auto& vertex : vertices)
    //{
    //  auto it = vertexToIndex.find(vertex);
    //  if (it != vertexToIndex.end())
    //  {
    //    mesh.indices.push_back(it->second);
    //  }
    //  else
    //  {
    //    mesh.vertices.push_back(vertex);
    //    mesh.indices.push_back(curIndex);
    //  }

    //  vertexToIndex[vertex] = curIndex++;
    //}

    return mesh;
  }
}