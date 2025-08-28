#pragma once

#include "entt/entt.hpp"
#include "imgui.h"
#include "core/inspect_functions.hpp"

#include "rfl.hpp"

namespace dg
{

  using namespace entt::literals;

  template <class T>
  struct Inspector
  {
    Inspector()
    {
      entt::meta<T>().template func<&Inspector<T>::inspectFields>("Inspector"_hs);
    }

    static void inspectFields(void* _pInspected)
    {
      ImGui::Separator();

      T* pInspected = static_cast<T*>(_pInspected);
      
      std::string text = std::string(entt::type_name<T>()) + " component :";
      ImGui::Text(text.c_str());

      auto view = rfl::to_view(*pInspected);
      view.apply([](const auto& field) {
        inspect(field.name(), *field.value());
      });
    }

  };
}
