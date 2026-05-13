#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "click_speed"

static auto const o = Option::create(THIS_ID)
                          ->setName("Click Player Speed")
                          ->setDescription("Randomly increases or decreases the player velocity everytime you jump.\n<cl>suggested by KGplayerA</c>")
                          ->setCategory(category::randoms)
                          ->setSillyTier(SillyTier::Low)
                          ->autoRegister();

class $modify(ClickSpeedPlayerObject, PlayerObject) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    bool pushButton(PlayerButton p0) {
        int rng = rng::fast();
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