#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static Option const o = {
    "no_jump",
    "Randomly Don't Jump",
    "When making an input in a level, there's a chance the player does not respond to it.\n<cy>Credit: GilanyKing12</c>",
    category::randoms,
    SillyTier::Low,
};
REGISTER_HORRIBLE_OPTION(o);

class $modify(NoJumpGJBaseGameLayer, GJBaseGameLayer) {
    struct Fields {
        bool enabled = options::get(o.id);
        int chance = options::get(o.id);
    };

    void handleButton(bool down, int button, bool isPlayer1) {
        auto f = m_fields.self();

        if (f->enabled) {
            if (button == 1) {
                int rnd = randng::fast();
                if (rnd <= f->chance) return GJBaseGameLayer::handleButton(false, button, isPlayer1);
            };
        };

        GJBaseGameLayer::handleButton(down, button, isPlayer1);
    };
};