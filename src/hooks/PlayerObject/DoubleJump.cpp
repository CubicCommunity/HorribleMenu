#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class $modify(DoubleJumpPlayerObject, PlayerObject) {
    struct Fields {
        bool enabled = options::get(key::double_jump);

        int m_jumps = 0;
    };

    bool init(int player, int ship, GJBaseGameLayer * gameLayer, CCLayer * layer, bool playLayer) {
        if (!PlayerObject::init(player, ship, gameLayer, layer, playLayer)) return false;

        auto f = m_fields.self();

        this->template addEventListener<OptionEventFilter>(
            [this, f](OptionEvent* ev) {
                f->enabled = ev->getToggled();
                return ListenerResult::Propagate;
            },
            key::double_jump
        );

        return true;
    };

    bool pushButton(PlayerButton p0) {
        auto f = m_fields.self();

        if (f->enabled) {
            if (p0 == PlayerButton::Jump) {
                if (m_isOnGround) f->m_jumps = 0;
                if (!m_isOnGround) f->m_jumps++;
            };

            m_isOnGround = f->m_jumps < 2;
        };

        return PlayerObject::pushButton(p0);
    };
};