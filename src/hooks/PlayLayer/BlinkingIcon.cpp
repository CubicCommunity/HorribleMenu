#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static Option const o = {
        "blinking_icon",
        "Blinking Icon",
        "Your icon will start to randomly blink.\n<cy>Credit: DragonixGD</c>",
        category::obstructive,
        SillyTier::Low,
};
HORRIBLE_REGISTER_OPTION(o);

class $modify(BlinkingIconPlayLayer, PlayLayer) {
    struct Fields {
        bool enabled = options::get(o.id);
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        if (m_fields->enabled) nextBlink();
    };

    void nextBlink() {
        auto delay = randng::get(2.f, 1.f);
        log::debug("scheduling blink in {}s", delay);

        if (m_fields->enabled) scheduleOnce(schedule_selector(BlinkingIconPlayLayer::blink), delay);
    };

    void blink(float) {
        if (m_fields->enabled) {
            log::debug("Blink!");

            if (m_player1) m_player1->playSpawnEffect();
            if (m_player2) m_player2->playSpawnEffect();
        };

        nextBlink();
    };
};