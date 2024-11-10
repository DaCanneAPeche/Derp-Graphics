#define VMA_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include "vulkan_renderer/renderer.hpp"
#include "_vulkan/vulkan_tool_box.hpp"
#include "core/application.hpp"
#include "components/sprite.hpp"
#include <glm/gtc/constants.hpp>

class MainScene : public dg::Scene
{
  public:
    void start() override
    {
      entt::entity rick = application->registry.create();
      comp::Sprite& sprite = application->registry.emplace<comp::Sprite>(rick);

      std::vector<dg::Vertex> vertices 
      {
        {{-0.5f, -0.5f}, {0.0f, 0.0f}},
          {{0.5f, -0.5f}, {1.0f, 0.0f}},
          {{0.5f, 0.5f}, {1.0f, 1.0f}},
          {{-0.5f, 0.5f}, {0.0f, 1.0f}}
      };

      std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};
      sprite.model = std::make_unique<dg::Model>(vertices, indices);
      sprite.transform.rotation = glm::pi<float>()/4;
      sprite.transform.translation = { 0.3, 0.5 };
      sprite.transform.scaling = {1.3, 1.0};
    }

};

class Game : public dg::Application
{
  public:
    Game(const dg::WindowInfo& windowInfo, const dg::ApplicationInfo& appInfo)
      : dg::Application(windowInfo, appInfo)
    {
      addScene<MainScene>(dg::config::Scenes::MAIN);

      renderer.render = [this](vk::CommandBuffer& commandBuffer, vk::PipelineLayout& pipelineLayout)
      {
        auto renderView = registry.view<comp::Sprite>();
        
        for (auto entity : renderView)
        {
          auto [sprite] = renderView.get(entity);

          dg::PushConstant push {
            .transform = sprite.transform.getMatrix(),
              .offset = sprite.transform.translation};
          commandBuffer.pushConstants(pipelineLayout,
              vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
              0, sizeof(dg::PushConstant), &push);

          sprite.model->bind(commandBuffer);
          sprite.model->draw(commandBuffer);
        }

      };
    }

    ~Game()
    {

    }
};

int main(void)
{
  dg::WindowInfo windowInfo {1000, 1000, "Hello, world !"};
  dg::ApplicationInfo appInfo {"Hello, world program !", {1, 0, 0}};
  
  Game game(windowInfo, appInfo);
  game.run();
}
