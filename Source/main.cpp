#define VMA_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES 
#include <iostream>
#include "vulkan_renderer/renderer.hpp"
#include "vulkan_renderer/shader_variable_manager.hpp"
#include "_vulkan/vulkan_tool_box.hpp"
#include "core/application.hpp"
#include "core/inputs.hpp"
#include "components/sprite.hpp"
#include "components/position.hpp"
#include "core/system.hpp"
#include "vulkan_renderer/shader_module.hpp"
#include "_vulkan/slang_compiler.hpp"

#include <glm/gtc/constants.hpp>
#include <rfl.hpp>
#include <rfl/json.hpp>

using namespace entt::literals;

struct Assets : public dg::AssetManager
{
  using dg::AssetManager::AssetManager;

  dg::TextureAsset leclerc = texture("./assets/textures/leclerc.jpg");
  dg::TextureAsset rick = texture("./assets/textures/nothing_suspicious.jpeg");
  dg::TextureAsset atlas = texture("./assets/textures/text_atlas_test.png");
};

class MainScene;

class Game : public dg::Application
{
  public:
    Assets assets {vulkanToolBox};
    bool showOnlyOutlines = false;
    dg::ShaderVariableManager ubo {vulkanToolBox, renderer};

    Game(const dg::WindowInfo& windowInfo, const dg::ApplicationInfo& appInfo)
      : dg::Application(windowInfo, appInfo)
    {
      dg::SlangCompiler spriteShader("./assets/shaders/slang/sprite.slang");

      renderer.pipelinesInfo = {
        dg::PipelineInfo {
            dg::Pl::sprites,
            spriteShader.get("vertexMain"),
            spriteShader.get("fragmentMain")
        },
        dg::PipelineInfo {
            dg::Pl::outline,
            spriteShader.get("vertexMain"),
            spriteShader.get("fragmentMain"),
            getOutlineConfig()
        },
      };

      spriteShader.reflect(renderer.shaderDescription);
      // renderer.shaderDescription.print();

      addScene<MainScene>(dg::config::Scenes::MAIN);

      vk::AttachmentReference colorAttachmentReference = renderer.renderPass.addAttachment(
          vulkanToolBox.getSwapChainSurfaceFormat().format,
          vk::SampleCountFlagBits::e1,
          vk::AttachmentLoadOp::eClear,
          vk::AttachmentStoreOp::eStore,
          vk::AttachmentLoadOp::eDontCare,
          vk::AttachmentStoreOp::eDontCare,
          vk::ImageLayout::eUndefined,
          vk::ImageLayout::ePresentSrcKHR
          ).get(vk::ImageLayout::eColorAttachmentOptimal);

      vk::AttachmentReference depthAttachmentReference = renderer.renderPass.addAttachment(
          vulkanToolBox.findDepthFormat(),
          vk::SampleCountFlagBits::e1,
          vk::AttachmentLoadOp::eClear,
          vk::AttachmentStoreOp::eDontCare,
          vk::AttachmentLoadOp::eDontCare,
          vk::AttachmentStoreOp::eDontCare,
          vk::ImageLayout::eUndefined,
          vk::ImageLayout::eDepthStencilAttachmentOptimal
          ).get(vk::ImageLayout::eDepthStencilAttachmentOptimal);

      renderer.renderPass.addSubpass(vk::SubpassDescription(
            {}, vk::PipelineBindPoint::eGraphics, {}, colorAttachmentReference, {},
            &depthAttachmentReference, {}
            ));

      renderer.renderPass.addDependency(vk::SubpassDependency(
            vk::SubpassExternal, 0,
            vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
            vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
            {},
            vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite,
            {}
            ));

      renderer.renderPass.create();
    }

    ~Game()
    {

    }

    void start() override
    {
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
      assets.processTextureLoadings(renderer.descriptors["textures"]);
      renderer.updateDescriptorSets();
      assets.textureDescriptorUpdates.clear();

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

      if(ImGui::TreeNode("Systems"))
      {
        ImGui::Text("Number of systems : %lu", dg::_systems::allSystems.size());

        for (const auto& system : dg::_systems::allSystems)
        {
          ImGui::Checkbox(std::string(system->name).c_str(), &system->active);
        }

        ImGui::TreePop();
      }

      ImGui::End();
    }
};

class MainScene : public dg::Scene
{
  private:
    entt::entity rick;
    Game* game = nullptr;

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
      game = static_cast<Game*>(app);

      // entt::meta<comp::Position>().func<&comp::Position::inspect>("Inspector"_hs);
      rick = app->registry.create();
      auto& pos = app->registry.emplace<comp::Position>(rick);
      pos.x = 10;
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
      sprite.textureId = game->assets.leclerc.loadAndGetIndex();
      // sprite.transform.ratio = assetManager->getTexture(sprite.textureId).getRatio();
      sprite.transform.ratio = 1;

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

      bindInput(dg::Key::m, dg::config::Signals::RICK_MOVE);

      signalHandler.on(dg::config::Signals::RICK_MOVE, [](){
        LOGD << "MOVE";  
      });

    }

    void update() override
    {
      if (isActionTakingPlace(dg::config::Signals::RICK_MOVE))
        app->registry.get<comp::Sprite>(rick).transform.translation.x += 1 * app->deltaTime;
    }
};


int main(void)
{
  dg::WindowInfo windowInfo {1000, 1000, "Hello, world !"};
  dg::ApplicationInfo appInfo {"Hello, world program !", {1, 0, 0}};

  Game game(windowInfo, appInfo);
  game.init();
  game.run();
}
