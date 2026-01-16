#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class $modify(ClickSpeedPlayerObject, PlayerObject) {
    struct Fields {
        bool enabled = options::get(key::click_speed);
    };

    bool pushButton(PlayerButton p0) {
        if (m_fields->enabled) {
            int rng = randng::fast() % 100;
            int currentSpeed = m_playerSpeed;

            log::debug("current speed: {}", currentSpeed);

            if (rng <= 50) {
                // increase the player speed
                m_playerSpeed = currentSpeed + 1;
            } else {
                // decrease the player speed
                m_playerSpeed = currentSpeed - 1;
            };

            log::debug("Click Speed modified player speed to {}", m_playerSpeed);
        };

        return PlayerObject::pushButton(p0);
    };
};