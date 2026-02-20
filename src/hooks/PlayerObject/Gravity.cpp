#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static Option const o = {
    "gravity",
    "Randomize Gravity",
    "Every time you jump in a level, gravity force will increase or decrease randomly.\n<cy>Credit: NJAgain</c>",
    category::randoms,
    SillyTier::Low,
};
REGISTER_HORRIBLE_OPTION(o);

class $modify(GravityPlayerObject, PlayerObject) {
    struct Fields {
        bool enabled = options::get(o.id);

        bool m_isOnSlope = false;
        bool m_wasOnSlope = false;
    };

    void updateJump(float p0) {
        auto f = m_fields.self();

        if (f->enabled) {
            float newGrav = std::round(randng::pc() * (2.5f) * 100.f) / 100.f;
            auto onGrnd = m_isOnGround || m_isOnGround2 || m_isOnGround3 || m_isOnGround4;

            // Only set gravity if on flat ground (not on a slope) and not rotating
            if (onGrnd && !m_isRotating && !f->m_isOnSlope && !f->m_wasOnSlope) {
                m_gravityMod = newGrav;
                log::debug("set gravity to x{} (flat ground)", newGrav);
            };
        };

        PlayerObject::updateJump(p0);
    };
};