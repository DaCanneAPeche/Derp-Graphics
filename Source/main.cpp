#define VMA_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include "vulkan_renderer/renderer.hpp"
#include "_vulkan/vulkan_tool_box.hpp"
#include "core/application.hpp"
#include "components/sprite.hpp"
#include "components/position.hpp"
#include <glm/gtc/constants.hpp>
#include <rfl.hpp>
#include <rfl/json.hpp>

class MainScene : public dg::Scene
{
  private:
    entt::entity rick;

  public:
    void start() override
    {
      rick = application->registry.create();
      comp::Sprite& sprite = application->registry.emplace<comp::Sprite>(rick);
      application->registry.emplace<comp::Position>(rick);

      std::vector<dg::Vertex> vertices 
      {
        {{-0.5f, -0.5f}, {0.0f, 0.0f}},
          {{0.5f, -0.5f}, {1.0f, 0.0f}},
          {{0.5f, 0.5f}, {1.0f, 1.0f}},
          {{-0.5f, 0.5f}, {0.0f, 1.0f}}
      };

      std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};
      sprite.model = std::make_unique<dg::Model>(vertices, indices,
          application->vulkanToolBox);
      sprite.transform.rotation = glm::pi<float>()/4;
      sprite.transform.translation = { 0.3, 0.5 };
      sprite.transform.scaling = {1.3, 1.0};
      sprite.textureId = 0;

      signalHandler.on(dg::config::Signals::KEY_PRESS, [this](int key, int scancode,
            int mods)
      {
        auto& sprite = application->registry.get<comp::Sprite>(rick);

        if (key == GLFW_KEY_W)
          sprite.transform.translation.y -= 0.1;
        else if (key == GLFW_KEY_S)
          sprite.transform.translation.y += 0.1;
        else if (key == GLFW_KEY_A)
          sprite.transform.translation.x -= 0.1;
        else if (key == GLFW_KEY_D)
          sprite.transform.translation.x += 0.1;
        else if (key == GLFW_KEY_UP)
          sprite.textureId++;
        else if (key == GLFW_KEY_DOWN)
          sprite.textureId--;

      });

      signalHandler.on(dg::config::Signals::MOUSE_PRESS, [this](int button,
            int mods)
      {
        if (button != GLFW_MOUSE_BUTTON_LEFT) return;

        auto& sprite = application->registry.get<comp::Sprite>(rick);
        sprite.transform.rotation += glm::pi<float>()/4;
      });
    }

};

class Game : public dg::Application
{
  public:
    bool showOnlyOutlines = false;

    Game(const dg::WindowInfo& windowInfo, const dg::ApplicationInfo& appInfo)
      : dg::Application(windowInfo, appInfo)
    {
      renderer.pipelinesInfo = {
        dg::PipelineInfo {
            dg::Pl::sprites,
            "./assets/compiled_shaders/shape.vert.spv",
            "./assets/compiled_shaders/shape.frag.spv",
        },
        dg::PipelineInfo {
            dg::Pl::outline,
            "./assets/compiled_shaders/shape.vert.spv",
            "./assets/compiled_shaders/shape.frag.spv",
            getOutlineConfig()
        },
      };

      addScene<MainScene>(dg::config::Scenes::MAIN);
    }

    ~Game()
    {

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
    
    void render() override
    {
      if (showOnlyOutlines)
        renderer.bindPipeline(dg::Pl::outline);
      else
        renderer.bindPipeline(dg::Pl::sprites);

      auto renderView = registry.view<comp::Sprite>();

      for (auto entity : renderView)
      {
        auto [sprite] = renderView.get(entity);
        sprite.draw(renderer);
      }

    }

    void imguiRender() override
    {
      ImGui::Begin("Debug");

      if (ImGui::TreeNode("Timing"))
      {
        ImGui::Text("Frame duration : %f ms", deltaTime.count());
        ImGui::Text("FPS : %d", static_cast<int>(1000/deltaTime.count()));
        ImGui::TreePop();
      }

      ImGui::Spacing();

      if (ImGui::TreeNode("Rendering"))
      {
        ImGui::Checkbox("Show only outlines", &showOnlyOutlines);
        unsigned long maxPushConstantSize =
          vulkanToolBox.physicalDevice.getProperties().limits.maxPushConstantsSize;
        ImGui::Text("Push constant size : %lu bits out of %lu",
            sizeof(dg::PushConstant), maxPushConstantSize);
        ImGui::TreePop();
      }

      ImGui::End();
    }
};

int main(void)
{
  std::cout << sizeof(dg::PipelineConfigInfo) << std::endl;
  dg::WindowInfo windowInfo {1000, 1000, "Hello, world !"};
  dg::ApplicationInfo appInfo {"Hello, world program !", {1, 0, 0}};

  const std::string json_windowInfo = rfl::json::write(windowInfo);
  const std::string json_appInfo = rfl::json::write(appInfo);

  std::cout << json_windowInfo << std::endl << json_appInfo << std::endl;
  
  Game game(windowInfo, appInfo);
  game.run();
}
