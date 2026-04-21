#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "double_jump"

static auto const o = Option::create(THIS_ID)
                          ->setName("Double-Jump")
                          ->setDescription("Allows your character to double-jump in a level.\n<cl>created by Cheeseworks</c>")
                          ->setCategory(category::misc)
                          ->setSillyTier(SillyTier::Low)
                          ->autoRegister();

class $modify(DoubleJumpPlayerObject, PlayerObject) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields {
        int m_jumps = 0;
    };

    bool init(int player, int ship, GJBaseGameLayer* gameLayer, CCLayer* layer, bool playLayer) {
        if (!PlayerObject::init(player, ship, gameLayer, layer, playLayer)) return false;

        auto f = m_fields.self();

        return true;
    };

    bool pushButton(PlayerButton p0) {
        auto f = m_fields.self();

        if (p0 == PlayerButton::Jump) {
            if (m_isOnGround) f->m_jumps = 0;
            if (!m_isOnGround) f->m_jumps++;
        };

        m_isOnGround = f->m_jumps < 2;

        return PlayerObject::pushButton(p0);
    };
};