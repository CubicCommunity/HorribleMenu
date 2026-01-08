#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class $modify(FlippedPlayLayer, PlayLayer) {
    struct Fields {
        bool enabled = options::get("flipped");
        int chance = options::getChance("flipped");

        bool flipping = false;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        if (m_fields->enabled) schedule(schedule_selector(FlippedPlayLayer::flip), 0.125f);
    };

    void flippingEnded() {
        m_fields->flipping = false;
        log::info("playlayer flipped");
    };

    void flip(float) {
        if (m_fields->enabled && !m_fields->flipping) {
            if (randng::tiny() > m_fields->chance) return;

            m_fields->flipping = true;
            log::debug("flipping the playlayer");

            auto action = CCSequence::createWithTwoActions(
                CCEaseSineOut::create(CCRotateBy::create(0.875f, 180.f)),
                CCCallFunc::create(this, callfunc_selector(FlippedPlayLayer::flippingEnded))
            );

            runAction(action);
        };
    };
};