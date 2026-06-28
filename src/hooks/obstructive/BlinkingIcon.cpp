#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "blinking_icon"

static auto const o = Option::create(THIS_ID)
                          ->setName("Blinking Icon")
                          ->setDescription("Your icon will start to randomly blink.\n<cl>suggested by DragonixGD</c>")
                          ->setCategory(category::obstructive)
                          ->setSillyTier(SillyTier::Low)
                          ->autoRegister();

class $modify(BlinkingIconPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    HORRIBLE_SETUP_INTERFACE_FUNC {
        if (!on) {
            unschedule(schedule_selector(BlinkingIconPlayLayer::blink));

            return;
        };

        nextBlink();
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        HORRIBLE_SETUP_INTERFACE_FUNC_NAME();
    };

    void nextBlink() {
        auto delay = rng::get(1.25f, 0.875f);
        log::trace("scheduling blink in {}s", delay);

        scheduleOnce(schedule_selector(BlinkingIconPlayLayer::blink), delay);
    };

    void blink(float) {
        log::debug("Blink!");

        if (m_player1) m_player1->playSpawnEffect();
        if (m_player2) m_player2->playSpawnEffect();

        nextBlink();
    };
};

HORRIBLE_TOGGLE_MODIFY(PlayLayer, BlinkingIconPlayLayer);