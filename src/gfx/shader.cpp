#include "shader.h"

#include <span>
#include <string_view>
#include <stdexcept>
#include <fstream>
#include <memory>

#include <glad/gl.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "utility/defer.h"

std::string LoadFile(std::string_view file)
{
  std::string actualPath = "assets/shaders/" + std::string(file);
  std::ifstream ifs(actualPath);
  return std::string((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
}

GLuint CompileShader(GLenum stage, std::string_view source)
{
  auto sourceStr = std::string(source);
  const GLchar* strings = sourceStr.c_str();

  GLuint shader = glCreateShader(stage);
  glShaderSource(shader, 1, &strings, nullptr);
  glCompileShader(shader);

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    GLsizei infoLength = 512;
    //glGetShaderInfoLog(shader, 0, &infoLength, nullptr);
    std::string infoLog(infoLength + 1, '\0');
    glGetShaderInfoLog(shader, infoLength, nullptr, infoLog.data());

    throw std::runtime_error(infoLog);
  }

  return shader;
}

void LinkProgram(GLuint program)
{
  glLinkProgram(program);
  GLsizei length = 512;

  GLint success{};
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success)
  {
    //GLsizei length;
    //glGetProgramInfoLog(program, 0, &length, nullptr);
    std::string infoLog(length + 1, '\0');
    glGetProgramInfoLog(program, length, nullptr, infoLog.data());

    throw std::runtime_error(infoLog);
  }
}

auto InitUniforms(GLuint program)
{
  decltype(GFX::Shader::uniforms) uniforms;

  GLint uniform_count = 0;
  glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniform_count);

  if (uniform_count > 0)
  {
    GLint max_name_len = 0;
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);

    std::string uniform_name(max_name_len + 1, '\0');

    for (GLint i = 0; i < uniform_count; ++i)
    {
      GLsizei length = 0;
      GLsizei count = 0;
      GLenum 	type = GL_NONE;
      glGetActiveUniform(program, i, max_name_len, &length, &count, &type, uniform_name.data());

      GLuint uniform_info = {};
      uniform_info = glGetUniformLocation(program, uniform_name.c_str());

      // using the regular string may result in extra NULs at the end, changing hashes!
      uniforms.emplace(uniform_name.c_str(), uniform_info);
    }
  }

  return uniforms;
}

namespace GFX
{
  void Shader::Bind() const
  {
    glUseProgram(program);
  }

  void Shader::SetBool(std::string_view uniform, bool value)
  {
    assert(uniforms.contains(uniform));
    glProgramUniform1i(program, uniforms.find(uniform)->second, static_cast<GLint>(value));
  }
  void Shader::SetInt(std::string_view uniform, int32_t value)
  {
    assert(uniforms.contains(uniform));
    glProgramUniform1i(program, uniforms.find(uniform)->second, value);
  }
  void Shader::SetUInt(std::string_view uniform, uint32_t value)
  {
    assert(uniforms.contains(uniform));
    glProgramUniform1ui(program, uniforms.find(uniform)->second, value);
  }
  void Shader::SetFloat(std::string_view uniform, float value)
  {
    assert(uniforms.contains(uniform));
    glProgramUniform1f(program, uniforms.find(uniform)->second, value);
  }
  void Shader::Set1FloatArray(std::string_view uniform, std::span<const float> value)
  {
    assert(uniforms.contains(uniform));
    glProgramUniform1fv(program, uniforms.find(uniform)->second, static_cast<GLsizei>(value.size()), value.data());
  }
  void Shader::Set2FloatArray(std::string_view uniform, std::span<const glm::vec2> value)
  {
    assert(uniforms.contains(uniform));
    glProgramUniform2fv(program, uniforms.find(uniform)->second, static_cast<GLsizei>(value.size()), glm::value_ptr(value.front()));
  }
  void Shader::Set3FloatArray(std::string_view uniform, std::span<const glm::vec3> value)
  {
    assert(uniforms.contains(uniform));
    glProgramUniform3fv(program, uniforms.find(uniform)->second, static_cast<GLsizei>(value.size()), glm::value_ptr(value.front()));
  }
  void Shader::Set4FloatArray(std::string_view uniform, std::span<const glm::vec4> value)
  {
    assert(uniforms.contains(uniform));
    glProgramUniform4fv(program, uniforms.find(uniform)->second, static_cast<GLsizei>(value.size()), glm::value_ptr(value.front()));
  }
  void Shader::SetIntArray(std::string_view uniform, std::span<const int> value)
  {
    assert(uniforms.contains(uniform));
    glProgramUniform1iv(program, uniforms.find(uniform)->second, static_cast<GLsizei>(value.size()), value.data());
  }
  void Shader::SetVec2(std::string_view uniform, const glm::vec2& value)
  {
    assert(uniforms.contains(uniform));
    glProgramUniform2fv(program, uniforms.find(uniform)->second, 1, glm::value_ptr(value));
  }
  void Shader::SetIVec2(std::string_view uniform, const glm::ivec2& value)
  {
    assert(uniforms.contains(uniform));
    glProgramUniform2iv(program, uniforms.find(uniform)->second, 1, glm::value_ptr(value));
  }
  void Shader::SetVec3(std::string_view uniform, const glm::vec3& value)
  {
    assert(uniforms.contains(uniform));
    glProgramUniform3fv(program, uniforms.find(uniform)->second, 1, glm::value_ptr(value));
  }
  void Shader::SetVec4(std::string_view uniform, const glm::vec4& value)
  {
    assert(uniforms.contains(uniform));
    glProgramUniform4fv(program, uniforms.find(uniform)->second, 1, glm::value_ptr(value));
  }
  void Shader::SetMat3(std::string_view uniform, const glm::mat3& mat)
  {
    assert(uniforms.contains(uniform));
    glProgramUniformMatrix3fv(program, uniforms.find(uniform)->second, 1, GL_FALSE, glm::value_ptr(mat));
  }
  void Shader::SetMat4(std::string_view uniform, const glm::mat4& mat)
  {
    assert(uniforms.contains(uniform));
    glProgramUniformMatrix4fv(program, uniforms.find(uniform)->second, 1, GL_FALSE, glm::value_ptr(mat));
  }
  void Shader::SetMat4Array(std::string_view uniform, std::span<const glm::mat4> mats)
  {
    assert(uniforms.contains(uniform));
    glProgramUniformMatrix4fv(program, uniforms.find(uniform)->second, static_cast<GLsizei>(mats.size()), GL_FALSE, glm::value_ptr(mats[0]));
  }

  Shader LoadVertexFragmentProgram(std::string_view vsFile, std::string_view fsFile)
  {
    std::string vertexSource = LoadFile(vsFile);
    std::string fragmentSource = LoadFile(fsFile);

    auto vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
    Defer a = [vertexShader]() { glDeleteShader(vertexShader); };

    auto fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
    Defer b = [fragmentShader]() { glDeleteShader(fragmentShader); };

    auto program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    try { LinkProgram(program); }
    catch (std::runtime_error& e) { glDeleteProgram(program); throw e; }

    return Shader
    {
      .program = program,
      .uniforms = InitUniforms(program)
    };
  }
}