#pragma once

#include "entt/entt.hpp"
#include "field_reflection.hpp"
#include "imgui.h"
#include "core/inspect_functions.hpp"

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
      T* pInspected = static_cast<T*>(_pInspected);
      
      ImGui::Text("Position component");

      field_reflection::for_each_field(*pInspected, [](std::string_view field, auto& value) {
          inspect(field, value);
          });

      ImGui::Separator();

    }

  };
}
