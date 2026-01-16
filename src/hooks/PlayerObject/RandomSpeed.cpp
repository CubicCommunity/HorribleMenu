#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class $modify(RandomSpeedPlayerObject, PlayerObject) {
    struct Fields {
        bool enabled = options::get(key::random_speed);
        int chance = options::getChance(key::random_speed);
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
