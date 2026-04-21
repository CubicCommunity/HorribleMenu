#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "pauses"

static auto const o = Option::create(THIS_ID)
                          ->setName("Random Pauses")
                          ->setDescription("While playing a level, it will randomly pause itself.\n<cl>suggested by DragonixGD</c>")
                          ->setCategory(category::randoms)
                          ->setSillyTier(SillyTier::Low)
                          ->autoRegister();

class $modify(PausePlayerObject, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields {
        unsigned int chance = options::getChance(THIS_ID);
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