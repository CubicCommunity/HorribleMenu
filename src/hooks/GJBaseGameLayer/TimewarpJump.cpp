#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

static constexpr auto id = "timewarp_jump";

static auto const o = Option::create(id)
                          .setName("Timewarp Jump")
                          .setDescription("When you jump, there's a chance time will briefly slow down or speed up.\n<cl>Credit: KGplayerA</c>")
                          .setCategory(category::randoms)
                          .setSillyTier(SillyTier::Low);
HORRIBLE_REGISTER_OPTION(o);

class $modify(TimewarpJumpGJBaseGameLayer, GJBaseGameLayer) {
    HORRIBLE_DELEGATE_HOOKS(id);

    void handleButton(bool down, int button, bool isPlayer1) {
        if (button == 1 && !down) {
            auto warpFactor = randng::get(2.f, 0.75f);

            log::debug("applying time warp factor {}", warpFactor);
            GJBaseGameLayer::updateTimeWarp(warpFactor);
        };

        GJBaseGameLayer::handleButton(down, button, isPlayer1);
    };
};