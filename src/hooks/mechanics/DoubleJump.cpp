#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "double_jump"

static auto const o = Option::create(THIS_ID)
                          ->setName("Double-Jump")
                          ->setDescription("Allows your character to double-jump in a level.\n<cl>created by Cheeseworks</c>")
                          ->setCategory(category::mechanics)
                          ->setSillyTier(SillyTier::Low)
                          ->setCheating(true)
                          ->autoRegister();

class $modify(DoubleJumpPlayerObject, PlayerObject) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        uint8_t jumps = 0;

        bool onGround = true;
    };

    bool pushButton(PlayerButton p0) {
        if (!m_gameLayer) return PlayerObject::pushButton(p0);

        auto f = m_fields.self();

        if (p0 == PlayerButton::Jump) {
            if (onGround()) f->jumps = 0;
            if (!onGround()) f->jumps++;
        };

        setOnGround(f->jumps < 2);

        return PlayerObject::pushButton(p0);
    };

    void hitGround(GameObject* object, bool notFlipped) {
        if (!m_gameLayer) return PlayerObject::hitGround(object, notFlipped);

        auto f = m_fields.self();

        auto wasOnGround = f->onGround;
        PlayerObject::hitGround(object, notFlipped);
        auto nowOnGround = onGround();

        if (m_hasEverJumped) {
            if (nowOnGround && !wasOnGround) {
                f->jumps = 0;
                setOnGround(true);
            };

            f->onGround = nowOnGround;
        };
    };

    bool onGround() const noexcept {
        return m_isOnGround && m_isOnGround2 && m_isOnGround3 && m_isOnGround4;
    };

    void setOnGround(bool onGround) {
        m_isOnGround = onGround;
        m_isOnGround2 = onGround;
        m_isOnGround3 = onGround;
        m_isOnGround4 = onGround;
    };
};