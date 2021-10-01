#pragma once

#include <functional>

struct MyEqual : public std::equal_to<>
{
  using is_transparent = void;
};

struct string_hash
{
  using is_transparent = void;
  using key_equal = std::equal_to<>;  // Pred to use
  using hash_type = std::hash<std::string_view>;  // just a helper local type
  size_t operator()(std::string_view txt) const { return hash_type{}(txt); }
  size_t operator()(const std::string& txt) const { return hash_type{}(txt); }
  size_t operator()(const char* txt) const { return hash_type{}(txt); }
};