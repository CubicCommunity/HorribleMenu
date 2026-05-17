#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "flipped"

static auto const o = Option::create(THIS_ID)
                          ->setName("FLIPPED")
                          ->setDescription("Sometimes while playing, your entire screen will just do a flip!\n<cl>created by Cheeseworks</c>")
                          ->setCategory(category::obstructive)
                          ->setSillyTier(SillyTier::High)
                          ->autoRegister();

class $modify(FlippedPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        uint8_t chance = options::getChance(THIS_ID);

        bool flipping = false;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        nextFlip();
    };

    void flippingEnded() {
        m_fields->flipping = false;
        log::info("playlayer flipped");
    };

    void nextFlip() {
        log::trace("scheduling flip");
        if (!m_hasCompletedLevel) scheduleOnce(schedule_selector(FlippedPlayLayer::flip), rng::get(10.f, 2.5f) * chanceToDelayPct(m_fields->chance));
    };

    void flip(float) {
        if (rng::fast() > m_fields->chance) runAction(CCEaseSineOut::create(CCRotateBy::create(0.875f, 180.f)));

        queueInMainThread([self = WeakRef(this)]() {
            if (auto s = self.lock()) s->nextFlip();
        });
    };
};