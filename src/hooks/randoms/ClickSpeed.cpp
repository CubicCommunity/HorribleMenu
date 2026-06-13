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
                          ->setCheating(true)
                          ->autoRegister();

class $modify(ClickSpeedPlayerObject, PlayerObject) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    bool pushButton(PlayerButton p0) {
        if (!m_gameLayer) return PlayerObject::pushButton(p0);

        auto currentSpeed = m_playerSpeed;
        log::trace("current speed: {}", currentSpeed);

        m_playerSpeed = rng::flip() ? currentSpeed + 1.f : currentSpeed - 1.f;

        log::debug("Click Speed modified player speed to {}", m_playerSpeed);

        return PlayerObject::pushButton(p0);
    };
};