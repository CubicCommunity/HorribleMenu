#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

static constexpr auto id = "pauses";

static auto const o = Option::create(id)
                          .setName("Random Pauses")
                          .setDescription("While playing a level, it will randomly pause itself.\n<cl>Credit: DragonixGD</c>")
                          .setCategory(category::randoms)
                          .setSillyTier(SillyTier::Low);
HORRIBLE_REGISTER_OPTION(o);

class $modify(PausePlayerObject, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(id);

    struct Fields {
        unsigned int chance = options::getChance(id);
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        nextPause();
    };

    void nextPause() {
        log::trace("scheduling pause");
        scheduleOnce(schedule_selector(PausePlayerObject::pause), randng::get(15.f, 3.f) * chanceToDelayPct(m_fields->chance));
    };

    void pause(float) {
        nextPause();
        pauseGame(true);
    };
};