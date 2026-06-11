#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "no_jump"

static auto const o = Option::create(THIS_ID)
                          ->setName("Randomly Don't Jump")
                          ->setDescription("When making an input in a level, there's a chance the player does not respond to it.\n<cl>suggested by GilanyKing12</c>")
                          ->setCategory(category::randoms)
                          ->setSillyTier(SillyTier::Low)
                          ->autoRegister();

class $modify(NoJumpGJBaseGameLayer, GJBaseGameLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        uint8_t chance = options::getChance(THIS_ID);
    };

    void handleButton(bool down, int button, bool isPlayer1) {
        if (down && button == 1) {
            if (rng::chance(m_fields->chance)) return GJBaseGameLayer::handleButton(false, button, isPlayer1);
        };

        GJBaseGameLayer::handleButton(down, button, isPlayer1);
    };
};