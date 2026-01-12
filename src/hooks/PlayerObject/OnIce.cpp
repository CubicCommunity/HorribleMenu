#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class $modify(OnIcePlayerObject, PlayerObject) {
    struct Fields {
        bool enabled = options::get("ice_level");
    };

    void update(float p0) {
        auto f = m_fields.self();

        // make the player always on ice. yea this is it xD
        m_isOnIce = f->enabled;
        PlayerObject::update(p0);
    };
};