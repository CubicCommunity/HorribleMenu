#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static constexpr auto id = "click_speed";

inline static Option const o = {
    id,
    "Click Player Speed",
    "Randomly increases or decreases the player speed everytime you jump.\n<cl>Credit: KGplayerA</c>",
    category::randoms,
    SillyTier::Medium,
};
HORRIBLE_REGISTER_OPTION(o);

class $modify(ClickSpeedPlayerObject, PlayerObject) {
    HORRIBLE_DELEGATE_HOOKS(id);

    bool pushButton(PlayerButton p0) {
        int rng = randng::fast() % 100;
        int currentSpeed = m_playerSpeed;

        log::trace("current speed: {}", currentSpeed);

        if (rng <= 50) {
            // increase the player speed
            m_playerSpeed = currentSpeed + 1;
        } else {
            // decrease the player speed
            m_playerSpeed = currentSpeed - 1;
        };

        log::debug("Click Speed modified player speed to {}", m_playerSpeed);

        return PlayerObject::pushButton(p0);
    };
};