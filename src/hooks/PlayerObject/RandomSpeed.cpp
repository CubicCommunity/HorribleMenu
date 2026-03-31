#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

static constexpr auto id = "random_speed";

static auto const o = Option::create(id)
                          .setName("Random Speed Change")
                          .setDescription("Randomly changes your speed while playing a level.\n<cl>suggested by imdissapearinghelp</c>")
                          .setCategory(category::randoms)
                          .setSillyTier(SillyTier::Medium);
HORRIBLE_REGISTER_OPTION(o);

class $modify(RandomSpeedPlayerObject, PlayerObject) {
    HORRIBLE_DELEGATE_HOOKS(id);

    struct Fields {
        unsigned int chance = options::getChance(id);
    };

    bool pushButton(PlayerButton button) {
        if (!PlayerObject::pushButton(button)) return false;

        auto f = m_fields.self();

        int rnd = randng::tiny();

        if (rnd <= f->chance) {
            // randomly choose a new speed between 10% and 200%
            m_playerSpeed = randng::get(200.f, 10.f) / 100.f;
            log::debug("Changed player speed to {}", m_playerSpeed);
        };

        return true;
    };
};
