#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static Option const o = {
    "random_mirror",
    "Random Mirror Portal",
    "Randomly activates a mirror portal while playing.\n<cy>Credit: TimeRed</c>",
    category::randoms,
    SillyTier::Low,
};
REGISTER_HORRIBLE_OPTION(o);

class $modify(RandomMirrorPlayLayer, PlayLayer) {
    struct Fields {
        bool enabled = options::get(o.id);
        int chance = options::getChance(o.id);

        bool isFlipped = false;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        if (m_fields->enabled) scheduleOnce(schedule_selector(RandomMirrorPlayLayer::nextFlipPortal), 0.125f);
    };

    void toggleFlipped(bool p0, bool p1) {
        auto f = m_fields.self();

        f->isFlipped = p0;
        log::debug("{}", f->isFlipped ? o.id : "unflipped");

        PlayLayer::toggleFlipped(p0, p1);
    };

    void nextFlipPortal(float) {
        log::debug("scheduling flip");
        if (m_fields->enabled) scheduleOnce(schedule_selector(RandomMirrorPlayLayer::flipPortal), randng::get(10.f, 1.f) * chanceToDelayPct(m_fields->chance));
    };

    void flipPortal(float) {
        auto f = m_fields.self();

        if (f->enabled) {
            toggleFlipped(!f->isFlipped, false);
            scheduleOnce(schedule_selector(RandomMirrorPlayLayer::nextFlipPortal), 2.5f);
        };
    };
};