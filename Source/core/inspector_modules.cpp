#include "core/inspector_modules.hpp"

namespace dg
{
  namespace inspector_modules
  {
    void time(float deltaTime)
    {
      if (ImGui::TreeNode("Timing"))
      {
        ImGui::Text("Frame duration : %f ms", deltaTime * 1000);
        ImGui::Text("FPS : %d", static_cast<int>(1 / deltaTime));
        ImGui::TreePop();
      }
    }

    void vulkanInfos(dg::VulkanToolBox& vulkanToolBox)
    {
      if (ImGui::TreeNode("Rendering"))
      {
        vk::PhysicalDeviceProperties physicalDeviceProperties =
          vulkanToolBox.physicalDevice.getProperties();

        unsigned long maxPushConstantSize =
          physicalDeviceProperties.limits.maxPushConstantsSize;
        ImGui::Text("Push constant size : %lu bits out of %lu",
            sizeof(dg::PushConstant), maxPushConstantSize);

        uint32_t rawApiVersion = physicalDeviceProperties.apiVersion;
        unsigned long apiVersion[4] = {
          vk::apiVersionVariant(rawApiVersion),
          vk::apiVersionMajor(rawApiVersion),
          vk::apiVersionMinor(rawApiVersion),
          vk::apiVersionPatch(rawApiVersion)
        };

        ImGui::Text("Hardware max API version : %lu.%lu.%lu.%lu", apiVersion[0],
            apiVersion[1], apiVersion[2], apiVersion[3]);

        ImGui::TreePop();
      }
    }

    void entities(entt::registry& registry)
    {
      if (ImGui::TreeNode("Entities"))
      {
        for (auto entity : registry.view<entt::entity>())
        {
          std::string label = "Entity " + std::to_string(static_cast<int>(entity));

          if (ImGui::TreeNode(label.c_str()))
          {
            for(auto [id, storage] : registry.storage())
            {
              auto type = entt::resolve(id);

              if (auto func = type.func(entt::hashed_string("Inspector"));
                  storage.contains(entity) && func) {
                void* comp = storage.value(entity);

                func.invoke(type.from_void(comp), comp);
              }
            }
            ImGui::TreePop();
          }
        }

        ImGui::TreePop();
      }
    }

    void systems()
    {
      if(ImGui::TreeNode("Systems"))
      {
        ImGui::Text("Number of systems : %lu", dg::_systems::allSystems.size());

        for (const auto& system : dg::_systems::allSystems)
        {
          if (ImGui::TreeNode(system->inspectorInfo.name.c_str()))
          {
            ImGui::Checkbox("Active", &system->active);

            ImGui::Separator();

            ImGui::Text("Components :");
            for (size_t i = 0 ; i < system->inspectorInfo.components.size() ; i++)
            {
              ImGui::Text(system->inspectorInfo.components[i].c_str());
            }

            ImGui::Separator();

            ImGui::Text("ECS signals :");
            if (system->areFunctionsOverriden.onCreation) ImGui::Text("- on entity creation");
            if (system->areFunctionsOverriden.onDestruct) ImGui::Text("- on entity destruction");
            if (system->areFunctionsOverriden.onReplace) ImGui::Text("- on entity replacement");

            ImGui::TreePop();
          }
        }

        ImGui::TreePop();
      }
    }

  }
}
