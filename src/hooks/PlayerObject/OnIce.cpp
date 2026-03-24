#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

static constexpr auto id = "ice_level";

static auto const o = Option::create(id)
                          .setName("Ice Level")
                          .setDescription("Makes every surface icy. Slip n' slide!\n<cl>Credit: TimeRed</c>")
                          .setCategory(category::misc)
                          .setSillyTier(SillyTier::Medium);
HORRIBLE_REGISTER_OPTION(o);

class $modify(OnIcePlayerObject, PlayerObject) {
    HORRIBLE_DELEGATE_HOOKS(id);

    void update(float p0) {
        // make the player always on ice. yea this is it xD
        m_isOnIce = true;
        PlayerObject::update(p0);
    };
};