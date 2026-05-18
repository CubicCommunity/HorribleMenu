#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "inverse_input"

static auto const o = Option::create(THIS_ID)
                          ->setName("Inversed Inputs")
                          ->setDescription("You jump while you're not holding the button, and don't jump while you hold the button. In platformer, horizontal movement inputs are switched with each other.\n<cl>suggested by ItsZentry</c>")
                          ->setCategory(category::misc)
                          ->setSillyTier(SillyTier::Low)
                          ->setCheating(true)
                          ->autoRegister();

class $modify(InverseInputGJBaseGameLayer, GJBaseGameLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    void handleButton(bool down, int button, bool isPlayer1) {
        if (!m_isPlatformer) return GJBaseGameLayer::handleButton(!down, button, isPlayer1);

        switch (button) {
            default: [[fallthrough]];  // who nose

            case 1: down = !down; break;
            case 2: button = 3; break;  // left -> right
            case 3: button = 2; break;  // right -> left
        };

        GJBaseGameLayer::handleButton(down, button, isPlayer1);
    };
};