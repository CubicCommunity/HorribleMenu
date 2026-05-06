#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "random_mirror"

static auto const o = Option::create(THIS_ID)
                          ->setName("Random Mirror Chance")
                          ->setDescription("Randomly activates a mirror portal while playing.\n<cl>suggested by TimeRed</c>")
                          ->setCategory(category::chances)
                          ->setSillyTier(SillyTier::Low)
                          ->autoRegister();

class $modify(RandomMirrorPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields {
        uint8_t chance = options::getChance(THIS_ID);

        bool isFlipped = false;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        scheduleOnce(schedule_selector(RandomMirrorPlayLayer::nextFlipPortal), 0.125f);
    };

    void toggleFlipped(bool p0, bool p1) {
        PlayLayer::toggleFlipped(p0, p1);

        auto f = m_fields.self();

        f->isFlipped = p0;
        log::debug("{}", f->isFlipped ? "flipped" : "unflipped");
    };

    void nextFlipPortal(float) {
        log::trace("scheduling flip");
        scheduleOnce(schedule_selector(RandomMirrorPlayLayer::flipPortal), randng::get(10.f, 1.f) * chanceToDelayPct(m_fields->chance));
    };

    void flipPortal(float) {
        auto f = m_fields.self();

        toggleFlipped(!f->isFlipped, false);
        scheduleOnce(schedule_selector(RandomMirrorPlayLayer::nextFlipPortal), 2.5f);
    };
};