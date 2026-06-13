#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "gambler"

static auto const o = Option::create(THIS_ID)
                          ->setName("Ending Gamble")
                          ->setDescription("When reaching 95% in a level, you have a 50/50 chance at randomly being blasted way far back.\n<cl>suggested by Timered</c>")
                          ->setCategory(category::misc)
                          ->setSillyTier(SillyTier::High)
                          ->setCheating(true)
                          ->autoRegister();

class $modify(GamblerPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        bool triggered = false;
    };

    HORRIBLE_SETUP_INTERFACE_FUNC {
        if (!on) {
            unschedule(schedule_selector(GamblerPlayLayer::gamblerCheck));

            return;
        };

        schedule(schedule_selector(GamblerPlayLayer::gamblerCheck), 0.125f);
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        setupHorribleInterface();

        return true;
    };

    void fullReset() {
        PlayLayer::fullReset();
        schedule(schedule_selector(GamblerPlayLayer::gamblerCheck), 0.125f);
        log::trace("gambler full reset");
    };

    void gamblerCheck(float) {
        // detect the moment the player first reaches or crosses 95
        if (getCurrentPercentInt() >= 95) {
            if (rng::flip()) {
                log::info("Gambler lost the bet!");

                sfx::play(sfx::file::pop);
                Notification::create("Unlucky!", NotificationIcon::Error)->show();

                m_player1->reversePlayer(nullptr);
                m_player2->reversePlayer(nullptr);

                m_player1->boostPlayer(rng::get(12.5f, 8.75f) * (m_player1->m_isUpsideDown ? -1.f : 1.f));
                m_player2->boostPlayer(rng::get(12.5f, 8.75f) * (m_player2->m_isUpsideDown ? -1.f : 1.f));
            } else {
                log::info("Gambler won the bet! instant win.");

                sfx::play(sfx::file::good);
                Notification::create("You got lucky this time...", NotificationIcon::Success)->show();

                levelComplete();
            };

            unschedule(schedule_selector(GamblerPlayLayer::gamblerCheck));
        };
    };
};

HORRIBLE_TOGGLE_MODIFY(PlayLayer, GamblerPlayLayer);