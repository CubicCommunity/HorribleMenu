#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static Option const o = {
        "pauses",
        "Random Pauses",
        "While playing a level, it will randomly pause itself.\n<cy>Credit: DragonixGD</c>",
        category::randoms,
        SillyTier::Low,
};
HORRIBLE_REGISTER_OPTION(o);

class $modify(PausePlayerObject, PlayLayer) {
    struct Fields {
        bool enabled = options::get(o.id);
        int chance = options::getChance(o.id);
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        if (m_fields->enabled) nextPause();
    };

    void nextPause() {
        log::debug("scheduling pause");
        if (m_fields->enabled) scheduleOnce(schedule_selector(PausePlayerObject::pause), randng::get(15.f, 3.f) * chanceToDelayPct(m_fields->chance));
    };

    void pause(float) {
        if (m_fields->enabled) {
            nextPause();
            pauseGame(true);
        };
    };
};