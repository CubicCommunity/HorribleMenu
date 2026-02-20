#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static Option const o = {
        "random_speed",
        "Random Speed Change",
        "Randomly changes your speed while playing a level.\n<cy>Credit: imdissapearinghelp</c>",
        category::randoms,
        SillyTier::Medium,
};
REGISTER_HORRIBLE_OPTION(o);

class $modify(RandomSpeedPlayerObject, PlayerObject) {
    struct Fields {
        bool enabled = options::get(o.id);
        int chance = options::getChance(o.id);
    };

    bool pushButton(PlayerButton button) {
        if (!PlayerObject::pushButton(button)) return false;

        auto f = m_fields.self();

        if (f->enabled) {
            int rnd = randng::tiny();

            if (rnd <= f->chance) {
                // randomly choose a new speed between 10% and 200%
                m_playerSpeed = randng::get(200.f, 10.f) / 100.f;
                log::debug("Changed player speed to {}", m_playerSpeed);
            };
        };

        return true;
    };
};
