#include "core/system.hpp"
#include "components/sprite.hpp"
#include "components/position.hpp"
#include "core/application.hpp"

class SpritePosSystem : public dg::MultiComponentsSystem<comp::Sprite, comp::Position>
{
  public:

    void update(dg::Scene& scene, entt::entity entity, comp::Sprite& sprite,
        comp::Position& pos) override
    {

    }

};

class SpriteSystem : public dg::SingleComponentSystem<comp::Sprite>
{
  public:

    void onCreation(dg::Scene& scene, entt::entity entity, comp::Sprite& sprite) override
    {
    }

    void update(dg::Scene& scene, entt::entity entity, comp::Sprite& sprite) override
    {
      sprite.transform.rotation += 0.01 * scene.app->deltaTime;
    }
};

DG_REGISTER_SYSTEM(SpritePosSystem)
DG_REGISTER_SYSTEM(SpriteSystem)
