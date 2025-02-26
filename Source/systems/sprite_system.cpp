#include "core/system.hpp"
#include "components/sprite.hpp"
#include "components/position.hpp"

class SpritePosSystem : public dg::System<comp::Sprite, comp::Position>
{
  public:

    void update(dg::Scene& scene, entt::entity entity, comp::Sprite& sprite,
        comp::Position& pos) override
    {

    }

};

class SpriteSystem : public dg::System<comp::Sprite> {};

DG_REGISTER_SYSTEM(SpritePosSystem)
DG_REGISTER_SYSTEM(SpriteSystem)
