#define VMA_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES 
#include <iostream>
#include "vulkan_renderer/renderer.hpp"
#include "_vulkan/vulkan_tool_box.hpp"
#include "core/application.hpp"
#include "core/inputs.hpp"
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
    std::unordered_map<uint32_t, std::string> getAssets() override
    {
      return {
        {0, "./assets/textures/leclerc.jpg"},
        {1, "./assets/textures/nothing_suspicious.jpeg"},
        {2, "./assets/textures/text_atlas_test.png"}
      };
    }

    void start() override
    {
      rick = app->registry.create();
      comp::Sprite& sprite = app->registry.emplace<comp::Sprite>(rick);

      std::vector<dg::Vertex> vertices 
      {
        {{-0.5f, -0.5f}, {0.0f, 0.0f}},
          {{0.5f, -0.5f}, {1.0f, 0.0f}},
          {{0.5f, 0.5f}, {1.0f, 1.0f}},
          {{-0.5f, 0.5f}, {0.0f, 1.0f}}
      };

      std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};
      sprite.model = std::make_unique<dg::Model>(vertices, indices,
          app->vulkanToolBox);
      sprite.transform.rotation = glm::pi<float>()/4;
      sprite.transform.translation = { 0.3, 0.5 };
      sprite.textureId = 1;
      sprite.transform.ratio = assetManager->getTexture(sprite.textureId).getRatio();

      signalHandler.on(dg::config::Signals::KEY_PRESS, [this](dg::Key key,
            dg::KeyboardMods mods)
      {
        auto& sprite = app->registry.get<comp::Sprite>(rick);

        if (key == dg::Key::w)
          sprite.transform.translation.y -= 0.1;
        else if (key == dg::Key::s)
          sprite.transform.translation.y += 0.1;
        else if (key == dg::Key::a)
          sprite.transform.translation.x -= 0.1;
        else if (key == dg::Key::d)
          sprite.transform.translation.x += 0.1;
        else if (key == dg::Key::up)
          sprite.textureId++;
        else if (key == dg::Key::down)
          sprite.textureId--;

      });

      signalHandler.on(dg::config::Signals::MOUSE_PRESS, [this](dg::MouseButton button,
            dg::KeyboardMods mods)
      {
        if (button != dg::MouseButton::left) return;

        auto& sprite = app->registry.get<comp::Sprite>(rick);
        sprite.transform.rotation += glm::pi<float>()/4;
      });

      bindInput(dg::Key::space, dg::config::Signals::RICK_ROTATE,
          dg::KeyboardMods::shift, dg::KeyboardMods::control);

      signalHandler.on(dg::config::Signals::RICK_ROTATE, [this](){
        auto& sprite = app->registry.get<comp::Sprite>(rick);
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
    
    void render(dg::Frame& frame) override
    {
      if (showOnlyOutlines)
        frame.bindPipeline(dg::Pl::outline);
      else
        frame.bindPipeline(dg::Pl::sprites);

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

      if (ImGui::TreeNode("Timing"))
      {
        ImGui::Text("Frame duration : %f ms", deltaTime);
        ImGui::Text("FPS : %d", static_cast<int>(1000 / deltaTime));
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

      ImGui::Spacing();

      if (ImGui::TreeNode("Entities"))
      {
        for (auto entity : registry.view<entt::entity>())
        {
          char label[8 + sizeof(int)];
          sprintf(label, "Entity %i", static_cast<int>(entity));

          if (ImGui::TreeNode(label))
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
      ImGui::End();
    }
};

int main(void)
{
  std::cout << sizeof(glm::mat2) << std::endl;
  std::cout << sizeof(glm::vec4) << std::endl;

  dg::WindowInfo windowInfo {1000, 1000, "Hello, world !"};
  dg::ApplicationInfo appInfo {"Hello, world program !", {1, 0, 0}};

  Game game(windowInfo, appInfo);
  game.run();
}
