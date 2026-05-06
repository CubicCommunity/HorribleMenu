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

    struct Fields {
        uint8_t chance = options::isEnabled(THIS_ID);
    };

    void handleButton(bool down, int button, bool isPlayer1) {
        auto f = m_fields.self();

        if (button == 1) {
            auto rnd = randng::fast();
            if (rnd <= f->chance) return GJBaseGameLayer::handleButton(false, button, isPlayer1);
        };

        GJBaseGameLayer::handleButton(down, button, isPlayer1);
    };
};