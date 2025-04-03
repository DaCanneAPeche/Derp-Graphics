#pragma once

#include <string>
#include <glm/gtc/constants.hpp>
#include "imgui.h"
#include "entt/meta/meta.hpp"
#include "core/transform2d.hpp"

namespace dg
{
  template <class T>
    void inspect(std::string_view name, T& var)
    {
      std::string text = std::string(entt::type_name<T>().value()) + ' ' + std::string(name) + ';';
      ImGui::Text(text.c_str());
    }

  template <>
    inline void inspect<>(std::string_view name, int& var)
    {
      ImGui::SliderInt(std::string(name).c_str(), &var, -100000, 100000);
    }

  template <>
    inline void inspect<>(std::string_view name, float& var)
    {
      ImGui::SliderFloat(std::string(name).c_str(), &var, -100000, 100000);
    }

  template <>
    inline void inspect<>(std::string_view name, Transform2d& var)
    {
      std::string text = std::string(name) + " :";
      ImGui::Text(std::string(text).c_str());
      ImGui::SliderFloat2("Position", reinterpret_cast<float*>(&var.translation), -1, 1);
      ImGui::SliderFloat("Rotation", &var.rotation, 0, 2 * glm::pi<float>());
      ImGui::SliderFloat2("Scale", reinterpret_cast<float*>(&var.scaling), -10, 10);
    }

}
