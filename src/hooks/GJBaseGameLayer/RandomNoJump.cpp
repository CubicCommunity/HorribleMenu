#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static constexpr auto id = "no_jump";

inline static Option const o = {
    id,
    "Randomly Don't Jump",
    "When making an input in a level, there's a chance the player does not respond to it.\n<cy>Credit: GilanyKing12</c>",
    category::randoms,
    SillyTier::Low,
};
HORRIBLE_REGISTER_OPTION(o);

class $modify(NoJumpGJBaseGameLayer, GJBaseGameLayer) {
    HORRIBLE_DELEGATE_HOOKS(id);

    struct Fields {
        int chance = options::get(id);
    };

    void handleButton(bool down, int button, bool isPlayer1) {
        auto f = m_fields.self();

        if (button == 1) {
            int rnd = randng::fast();
            if (rnd <= f->chance) return GJBaseGameLayer::handleButton(false, button, isPlayer1);
        };

        GJBaseGameLayer::handleButton(down, button, isPlayer1);
    };
};