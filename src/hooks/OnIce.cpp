#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "ice_level"

static auto const o = Option::create(THIS_ID)
                          ->setName("Ice Level")
                          ->setDescription("Makes every surface icy in platformer mode. Slip n' slide!\n<cl>suggested by TimeRed</c>")
                          ->setCategory(category::misc)
                          ->setSillyTier(SillyTier::Low)
                          ->setCheating(true)
                          ->autoRegister();

class $modify(OnIcePlayerObject, PlayerObject) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    void update(float p0) {
        PlayerObject::update(p0);
        m_isOnIce = true;  // make the player always on ice. yea this is it xD
    };
};