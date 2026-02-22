#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static constexpr auto id = "ice_level";

inline static Option const o = {
    id,
    "Ice Level",
    "Makes every surface icy. Slip n' slide!\n<cy>Credit: TimeRed</c>",
    category::misc,
    SillyTier::Medium,
};
HORRIBLE_REGISTER_OPTION(o);

class $modify(OnIcePlayerObject, PlayerObject) {
    HORRIBLE_DELEGATE_HOOKS(id);

    void update(float p0) {
        // make the player always on ice. yea this is it xD
        m_isOnIce = true;
        PlayerObject::update(p0);
    };
};