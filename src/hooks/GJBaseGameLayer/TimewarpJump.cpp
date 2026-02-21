#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static Option const o = {
    "timewarp_jump",
    "Timewarp Jump",
    "When you jump, there's a chance time will briefly slow down or speed up.\n<cy>Credit: KGplayerA</c>",
    category::randoms,
    SillyTier::Low,
};
HORRIBLE_REGISTER_OPTION(o);

class $modify(TimewarpJumpGJBaseGameLayer, GJBaseGameLayer) {
    struct Fields {
        bool enabled = options::get(o.id);
    };

    void handleButton(bool down, int button, bool isPlayer1) {
        if (m_fields->enabled) {
            if (button == 1 && !down) {
                auto warpFactor = randng::get(300.f, 50.f) / 100.f;

                log::debug("applying time warp factor {}", warpFactor);
                GJBaseGameLayer::updateTimeWarp(warpFactor);
            };
        };

        GJBaseGameLayer::handleButton(down, button, isPlayer1);
    };
};