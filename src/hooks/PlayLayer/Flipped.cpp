#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

static constexpr auto id = "flipped";

static auto const o = Option::create(id)
                          .setName("FLIPPED")
                          .setDescription("Sometimes while playing, your entire screen will just do a flip!\n<cl>created by Cheeseworks</c>")
                          .setCategory(category::obstructive)
                          .setSillyTier(SillyTier::High);
HORRIBLE_REGISTER_OPTION(o);

class $modify(FlippedPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(id);

    struct Fields {
        unsigned int chance = options::getChance(id);

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
                CCCallFunc::create(this, callfunc_selector(FlippedPlayLayer::flippingEnded)));

            runAction(action);
        };
    };
};