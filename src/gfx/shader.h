#pragma once

#include <span>
#include <unordered_map>
#include <string_view>

#include <glm/fwd.hpp>

#include "utility/transparent_string_hash.h"

namespace GFX
{
  struct Shader
  {
    void Bind() const;

    void SetBool(std::string_view uniform, bool value);
    void SetInt(std::string_view uniform, int32_t value);
    void SetUInt(std::string_view uniform, uint32_t value);
    void SetFloat(std::string_view uniform, float value);
    void Set1FloatArray(std::string_view uniform, std::span<const float> value);
    void Set2FloatArray(std::string_view uniform, std::span<const glm::vec2> value);
    void Set3FloatArray(std::string_view uniform, std::span<const glm::vec3> value);
    void Set4FloatArray(std::string_view uniform, std::span<const glm::vec4> value);
    void SetIntArray(std::string_view uniform, std::span<const int> value);
    void SetVec2(std::string_view uniform, const glm::vec2& value);
    void SetIVec2(std::string_view uniform, const glm::ivec2& value);
    void SetVec3(std::string_view uniform, const glm::vec3& value);
    void SetVec4(std::string_view uniform, const glm::vec4& value);
    void SetMat3(std::string_view uniform, const glm::mat3& mat);
    void SetMat4(std::string_view uniform, const glm::mat4& mat);
    void SetMat4Array(std::string_view uniform, std::span<const glm::mat4> mats);

    uint32_t program{};
    std::unordered_map<std::string, int32_t, string_hash, MyEqual> uniforms{};
  };

  Shader LoadVertexFragmentProgram(std::string_view vsFile, std::string_view fsFile);
}