#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "pauses"

static auto const o = Option::create(THIS_ID)
                          ->setName("Random Pause Chance")
                          ->setDescription("While playing a level, it will randomly pause itself.\n<cl>suggested by DragonixGD</c>")
                          ->setCategory(category::chances)
                          ->setSillyTier(SillyTier::Low)
                          ->setCheating(true)
                          ->autoRegister();

class $modify(PausePlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        uint8_t chance = options::getChance(THIS_ID);
    };

    HORRIBLE_SETUP_INTERFACE_FUNC {
        if (!on) {
            unschedule(schedule_selector(PausePlayLayer::pause));

            return;
        };

        nextPause();
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        HORRIBLE_SETUP_INTERFACE_FUNC_NAME();
    };

    void nextPause() {
        log::trace("scheduling pause");
        scheduleOnce(schedule_selector(PausePlayLayer::pause), rng::get(15.f, 3.f) * chanceToDelayPct(m_fields->chance));
    };

    void pause(float) {
        nextPause();
        pauseGame(true);
    };
};

HORRIBLE_TOGGLE_MODIFY(PlayLayer, PausePlayLayer);