#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class $modify(RandomMirrorPlayLayer, PlayLayer) {
    struct Fields {
        bool enabled = options::get("random_mirror");
        int chance = options::getChance("random_mirror");

        bool isFlipped = false;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        if (m_fields->enabled) scheduleOnce(schedule_selector(RandomMirrorPlayLayer::nextFlipPortal), 0.125f);
    };

    void toggleFlipped(bool p0, bool p1) {
        auto f = m_fields.self();

        f->isFlipped = p0;
        log::debug("{}", f->isFlipped ? "flipped" : "unflipped");

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