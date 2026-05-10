#pragma once

#include "core/application.hpp"
#include "vulkan_renderer/shader_variable_manager.hpp"
#include "_vulkan/slang_compiler.hpp"
#include "components/sprite.hpp"
#include "components/position.hpp"
#include "core/system.hpp"
#include "core/config_info.hpp"
#include "vulkan_renderer/render_pass_builder.hpp"

#include "assets.hpp"
#include "pipelines.hpp"

class Game : public dg::Application
{
  public:
    bool showOnlyOutlines = false;
    dg::ShaderVariableManager ubo {vulkanToolBox, renderer};

    Game(const dg::ConfigInfo& configInfo) : dg::Application(configInfo)
    {
      dg::SlangCompiler spriteShader("./assets/shaders/slang/sprite.slang");

      renderer.registerPipelineInfo(Pipelines::Sprites, spriteShader.get("vertexMain"),
          spriteShader.get("fragmentMain"));

      renderer.registerPipelineInfo(Pipelines::Outline, spriteShader.get("vertexMain"),
          spriteShader.get("fragmentMain"), getOutlineConfig());

      spriteShader.reflect(renderer.shaderDescription);
      renderer.shaderDescription.print();

      dg::RenderPassBuilder::BuildDefaultRenderpass(renderer.renderPass);
    }

    ~Game()
    {

    }

    void start() override
    {
      dg::AssetPack::initAssetManager(vulkanToolBox);

      vk::DescriptorImageInfo samplerInfo(renderer.m_imageSampler, {},
          vk::ImageLayout::eShaderReadOnlyOptimal);
      renderer.descriptors["textureSampler"].writeToImage(samplerInfo);

      ubo.init("ubo");
      vk::DescriptorBufferInfo uboInfo = ubo.uniformBuffer->descriptorInfo();
      renderer.descriptors["ubo"].writeToBuffer(uboInfo);

      renderer.updateDescriptorSets();

      renderer.window.resizeCallback = [this](GLFWwindow*, int, int)
      {
        vk::Extent2D extent = renderer.window.getVkExtent();

        dg::Transform2d transform;
        transform.ratio = float(extent.height) / float(extent.width);
        glm::mat2 transformMatrix = transform.getMatrix();
        dg::UniformBufferObject uboValue {
          glm::vec2(transformMatrix[0][0], transformMatrix[1][0]),
            glm::vec2(transformMatrix[0][1], transformMatrix[1][1])
        };

        ubo["screenTransform"].setValue(uboValue);
        ubo.processWrites();
      };
    }

    std::shared_ptr<dg::PipelineConfigInfo> getOutlineConfig()
    {
      std::shared_ptr<dg::PipelineConfigInfo> configInfo =
        std::make_shared<dg::PipelineConfigInfo>();
      dg::Pipeline::defaultPipelineConfigInfo(*configInfo);

      configInfo->rasterizationInfo = vk::PipelineRasterizationStateCreateInfo(
				{}, vk::False, vk::False, vk::PolygonMode::eLine,
				vk::CullModeFlagBits::eNone, vk::FrontFace::eClockwise, vk::False,
				0.0f, 0.0f, 0.0f, 1.0f
          );

      return configInfo;
    }

    void render(dg::Frame& frame) override
    {
      dg::AssetPack::getAssetManager()->processTextureLoadings(renderer.descriptors["textures"]);
      renderer.updateDescriptorSets();
      dg::AssetPack::getAssetManager()->textureDescriptorUpdates.clear();

      if (showOnlyOutlines)
        frame.bindPipeline(Pipelines::Outline);
      else
        frame.bindPipeline(Pipelines::Sprites);

      auto renderView = registry.view<comp::Sprite>();

      for (auto entity : renderView)
      {
        auto [sprite] = renderView.get(entity);
        sprite.draw(frame);
      }

    }

    // TODO : have blocks already  constructed that I can add to the imgui window
    void imguiRender() override
    {
      ImGui::Begin("Debug");

      if (ImGui::TreeNode("Timing"))
      {
        ImGui::Text("Frame duration : %f ms", deltaTime * 1000);
        ImGui::Text("FPS : %d", static_cast<int>(1 / deltaTime));
        ImGui::TreePop();
      }

      if (ImGui::TreeNode("Rendering"))
      {
        vk::PhysicalDeviceProperties physicalDeviceProperties =
          vulkanToolBox.physicalDevice.getProperties();

        ImGui::Checkbox("Wireframe", &showOnlyOutlines);

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

      ImGui::End();
    }
};
