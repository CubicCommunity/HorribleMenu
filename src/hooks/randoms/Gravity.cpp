#include <Util.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "gravity"

static auto const o = Option::create(THIS_ID)
                          ->setName("Randomize Gravity")
                          ->setDescription("Every time you jump in a level, gravity force will increase or decrease randomly.\n<cl>suggested by NJAgain</c>")
                          ->setCategory(category::randoms)
                          ->setSillyTier(SillyTier::Low)
                          ->setCheating(true)
                          ->autoRegister();

class $modify(GravityPlayerObject, PlayerObject) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        bool m_isOnSlope = false;
        bool m_wasOnSlope = false;
    };

    void updateJump(float p0) {
        if (!m_gameLayer) return PlayerObject::updateJump(p0);

        auto f = m_fields.self();

        auto newGrav = std::round(rng::pc() * (2.5f) * 100.f) / 100.f;
        auto onGrnd = m_isOnGround || m_isOnGround2 || m_isOnGround3 || m_isOnGround4;

        // Only set gravity if on flat ground (not on a slope) and not rotating
        if (onGrnd && !m_isRotating && !f->m_isOnSlope && !f->m_wasOnSlope) {
            m_gravityMod = newGrav;
            log::trace("set gravity to x{} (flat ground)", newGrav);
        };

        PlayerObject::updateJump(p0);
    };
};