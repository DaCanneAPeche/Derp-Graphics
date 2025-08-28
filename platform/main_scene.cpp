#include "game.hpp"
#include "core/scene_registration.hpp"

class MainScene : public dg::Scene
{
  private:
    entt::entity rick;
    Game* game = nullptr;

  public:

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
      sprite.textureId = game->assets.rick.loadAndGetIndex();
      sprite.transform.ratio = game->assets.rick.imageRatio;

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
            dg::KeyboardMods mods, glm::vec<2, double> mousePosition)
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

static dg::RegisterScene<MainScene> _(0);
