#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static constexpr auto id = "double_jump";

inline static Option const o = {
    id,
    "Double-Jump",
    "Allows your character to double-jump in a level.\n<cl>Credit: Cheeseworks</c>",
    category::misc,
    SillyTier::Low,
};
HORRIBLE_REGISTER_OPTION(o);

class $modify(DoubleJumpPlayerObject, PlayerObject) {
    HORRIBLE_DELEGATE_HOOKS(id);

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