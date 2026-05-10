#include "core/system.hpp"
#include "components/sprite.hpp"
#include "components/position.hpp"
#include "core/application.hpp"

class SpritePosSystem : public dg::System<comp::Sprite, comp::Position>
{
  public:

    void update(dg::Scene& scene, entt::entity entity, comp::Sprite& sprite,
        comp::Position& pos) override
    {

    }

};

class SpriteSystem : public dg::System<comp::Sprite>
{
  public:

    void onCreation(dg::Scene& scene, entt::entity entity, comp::Sprite& sprite) override
    {
      LOGD << "Sprite system created !";
    }

    void update(dg::Scene& scene, entt::entity entity, comp::Sprite& sprite) override
    {
      sprite.transform.rotation += 0.01 * scene.app->deltaTime;
    }
};

static dg::RegisterSystem<SpritePosSystem, dg::System<comp::Sprite, comp::Position>> _;
static dg::RegisterSystem<SpriteSystem, dg::System<comp::Sprite>> __;

