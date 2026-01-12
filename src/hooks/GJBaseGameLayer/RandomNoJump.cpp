#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class $modify(NoJumpGJBaseGameLayer, GJBaseGameLayer) {
    struct Fields {
        bool enabled = options::get("no_jump");
        int chance = options::get("no_jump");
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