#pragma once

#include "components/sprite.hpp"
#include "components/position.hpp"

#include "core/system.hpp"
#include "core/config_info.hpp"
#include "core/inspector_modules.hpp"
#include "core/application.hpp"
#include "core/shader_watcher.hpp"

#include "vulkan_renderer/slang_compiler.hpp"
#include "vulkan_renderer/render_pass_builder.hpp"
#include "vulkan_renderer/shader_variable_manager.hpp"

#include "assets.hpp"
#include "pipelines.hpp"

class Game : public dg::Application
{
  public:
    bool showOnlyOutlines = false;
    dg::ShaderVariableManager ubo {vulkanToolBox, renderer};
    dg::ShaderWatcher shaderWatcher;

    Game(const dg::ConfigInfo& configInfo) : dg::Application(configInfo)
    {
      shaderWatcher.initRenderer(renderer);
      shaderWatcher.watchFile("./assets/shaders/slang/sprite.slang");
      shaderWatcher.callback = [this]()
      {
        dg::SlangCompiler spriteShader("./assets/shaders/slang/sprite.slang");

        renderer.registerPipelineInfo(Pipelines::Sprites, spriteShader.get("vertexMain"),
            spriteShader.get("fragmentMain"));

        renderer.registerPipelineInfo(Pipelines::Outline, spriteShader.get("vertexMain"),
            spriteShader.get("fragmentMain"), getOutlineConfig());

        renderer.shaderDescription.clear();
        spriteShader.reflect(renderer.shaderDescription);
        renderer.shaderDescription.print();
      };
      shaderWatcher.callback();

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

    void update() override
    {
      shaderWatcher.process();
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

    void imguiRender() override
    {
      ImGui::Begin("Debug");
      
      ImGui::Checkbox("Wireframe rendering", &showOnlyOutlines);

      dg::inspector_modules::time(deltaTime);
      dg::inspector_modules::vulkanInfos(vulkanToolBox);
      dg::inspector_modules::entities(registry);
      dg::inspector_modules::systems();

      ImGui::End();
    }
};
