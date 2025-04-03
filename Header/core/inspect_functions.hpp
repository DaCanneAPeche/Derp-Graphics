#pragma once

#include <string>
#include "imgui.h"
#include "entt/meta/meta.hpp"

namespace dg
{
  template <class T>
  void inspect(std::string_view name, T& var)
  {
    std::string text = std::string(entt::type_name<T>().value()) + ' ' + std::string(name) + ';';
    ImGui::Text(text.c_str());
  }
}
