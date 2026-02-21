#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static Option const o = {
    "ice_level",
    "Ice Level",
    "Makes every surface icy. Slip n' slide!\n<cy>Credit: TimeRed</c>",
    category::misc,
    SillyTier::Medium,
};
HORRIBLE_REGISTER_OPTION(o);

class $modify(OnIcePlayerObject, PlayerObject) {
    struct Fields {
        bool enabled = options::get(o.id);
    };

    void update(float p0) {
        auto f = m_fields.self();

        // make the player always on ice. yea this is it xD
        m_isOnIce = f->enabled;
        PlayerObject::update(p0);
    };
};