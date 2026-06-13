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

    struct Fields final {
        uint8_t chance = options::getChance(THIS_ID);

        bool isFlipped = false;
    };

    HORRIBLE_SETUP_INTERFACE_FUNC {
        if (!on) {
            unschedule(schedule_selector(RandomMirrorPlayLayer::nextFlipPortal));
            unschedule(schedule_selector(RandomMirrorPlayLayer::flipPortal));

            return;
        };

        scheduleOnce(schedule_selector(RandomMirrorPlayLayer::nextFlipPortal), 0.125f);
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        setupHorribleInterface();
    };

    void toggleFlipped(bool p0, bool p1) {
        PlayLayer::toggleFlipped(p0, p1);

        auto f = m_fields.self();

        f->isFlipped = p0;
        log::debug("{}", f->isFlipped ? "flipped" : "unflipped");
    };

    void nextFlipPortal(float) {
        log::trace("scheduling flip");
        scheduleOnce(schedule_selector(RandomMirrorPlayLayer::flipPortal), rng::get(10.f, 1.f) * chanceToDelayPct(m_fields->chance));
    };

    void flipPortal(float) {
        toggleFlipped(!m_fields->isFlipped, false);
        scheduleOnce(schedule_selector(RandomMirrorPlayLayer::nextFlipPortal), 2.5f);
    };
};

HORRIBLE_TOGGLE_MODIFY(PlayLayer, RandomMirrorPlayLayer);