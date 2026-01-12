#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class $modify(GravityPlayerObject, PlayerObject) {
    struct Fields {
        bool enabled = options::get("gravity");

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