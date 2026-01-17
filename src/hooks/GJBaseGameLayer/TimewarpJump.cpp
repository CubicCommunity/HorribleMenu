#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class $modify(TimewarpJumpGJBaseGameLayer, GJBaseGameLayer) {
    struct Fields {
        bool enabled = options::get(key::timewarp_jump);
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