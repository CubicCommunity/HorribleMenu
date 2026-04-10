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
                          ->setSillyTier(SillyTier::Low);
HORRIBLE_REGISTER_OPTION(o);

class $modify(BlinkingIconPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        nextBlink();
    };

    void nextBlink() {
        auto delay = randng::get(2.f, 1.f);
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