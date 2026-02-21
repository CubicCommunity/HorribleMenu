#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static Option const o = {
    "flipped",
    "FLIPPED",
    "Sometimes while playing, your entire screen will just do a flip!\n<cy>Credit: Cheeseworks</c>",
    category::obstructive,
    SillyTier::High,
};
HORRIBLE_REGISTER_OPTION(o);

class $modify(FlippedPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(o.id);

    struct Fields {
        int chance = options::getChance(o.id);

        bool flipping = false;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        schedule(schedule_selector(FlippedPlayLayer::flip), 0.125f);
    };

    void flippingEnded() {
        m_fields->flipping = false;
        log::info("playlayer flipped");
    };

    void flip(float) {
        auto f = m_fields.self();

        if (!f->flipping) {
            if (randng::tiny() > f->chance) return;

            f->flipping = true;
            log::debug("flipping the playlayer");

            auto action = CCSequence::createWithTwoActions(
                CCEaseSineOut::create(CCRotateBy::create(0.875f, 180.f)),
                CCCallFunc::create(this, callfunc_selector(FlippedPlayLayer::flippingEnded))
            );

            runAction(action);
        };
    };
};