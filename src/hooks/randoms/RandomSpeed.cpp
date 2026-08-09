#include <Util.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "random_speed"

static auto const o = Option::create(THIS_ID)
                          ->setName("Random Speed Change")
                          ->setDescription("Randomly changes your speed while playing a level.\n<cl>suggested by imdissapearinghelp</c>")
                          ->setCategory(category::randoms)
                          ->setSillyTier(SillyTier::Medium)
                          ->setCheating(true)
                          ->autoRegister();

class $modify(RandomSpeedPlayerObject, PlayerObject) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        uint8_t chance = options::getChance(THIS_ID);
    };

    bool pushButton(PlayerButton button) {
        if (!PlayerObject::pushButton(button)) return false;

        auto f = m_fields.self();

        if (rng::chance(f->chance)) {
            // randomly choose a new speed between 10% and 200%
            m_playerSpeed = rng::get(200.f, 10.f) / 100.f;
            log::debug("Changed player speed to {}", m_playerSpeed);
        };

        return true;
    };
};
