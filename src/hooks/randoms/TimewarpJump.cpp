#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "timewarp_jump"

static auto const o = Option::create(THIS_ID)
                          ->setName("Timewarp Jump")
                          ->setDescription("When you jump, there's a chance time will briefly slow down or speed up.\n<cl>suggested by KGplayerA</c>")
                          ->setCategory(category::randoms)
                          ->setSillyTier(SillyTier::Low)
                          ->autoRegister();

class $modify(TimewarpJumpGJBaseGameLayer, GJBaseGameLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    void handleButton(bool down, int button, bool isPlayer1) {
        if (button == 1 && !down) {
            auto warpFactor = rng::get(2.f, 0.75f);

            log::debug("applying time warp factor {}", warpFactor);
            updateTimeWarp(warpFactor);
        };

        GJBaseGameLayer::handleButton(down, button, isPlayer1);
    };
};