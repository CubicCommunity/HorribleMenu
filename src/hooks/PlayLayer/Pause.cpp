#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static constexpr auto id = "pauses";

inline static Option const o = {
    id,
    "Random Pauses",
    "While playing a level, it will randomly pause itself.\n<cy>Credit: DragonixGD</c>",
    category::randoms,
    SillyTier::Low,
};
HORRIBLE_REGISTER_OPTION(o);

class $modify(PausePlayerObject, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(id);

    struct Fields {
        int chance = options::getChance(id);
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        nextPause();
    };

    void nextPause() {
        log::debug("scheduling pause");
        scheduleOnce(schedule_selector(PausePlayerObject::pause), randng::get(15.f, 3.f) * chanceToDelayPct(m_fields->chance));
    };

    void pause(float) {
        nextPause();
        pauseGame(true);
    };
};