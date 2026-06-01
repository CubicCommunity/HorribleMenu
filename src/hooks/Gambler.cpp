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

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        // check every frame so we can detect each percentage change
        if (!m_fields->triggered) schedule(schedule_selector(GamblerPlayLayer::gamblerCheck), 0.125f);

        return true;
    };

    // ensure that triggered is reset on level restart/full reset
    void fullReset() {
        PlayLayer::fullReset();
        m_fields->triggered = false;
        log::trace("gambler full reset");
    };

    void resetLevel() {
        PlayLayer::resetLevel();
        m_fields->triggered = false;
        log::trace("gambler level reset");
    };

    void resetLevelFromStart() {
        PlayLayer::resetLevelFromStart();
        m_fields->triggered = false;
        log::trace("gambler level reset from start");
    };

    void gamblerCheck(float) {
        auto f = m_fields.self();

        if (f->triggered) return;

        // detect the moment the player first reaches or crosses 95
        if (getCurrentPercentInt() >= 95) {
            if (rng::flip()) {
                log::info("Gambler lost the bet!");

                sfx::play(sfx::file::pop);
                Notification::create("Unlucky!", NotificationIcon::Error)->show();

                m_player1->reversePlayer(nullptr);
                m_player2->reversePlayer(nullptr);

                m_player1->boostPlayer(rng::get(12.5f, 8.75f));
                m_player2->boostPlayer(rng::get(12.5f, 8.75f));
            } else {
                log::info("Gambler won the bet! instant win.");

                sfx::play(sfx::file::good);
                Notification::create("You got lucky this time...", NotificationIcon::Success)->show();

                levelComplete();
            };

            f->triggered = true;
            unschedule(schedule_selector(GamblerPlayLayer::gamblerCheck));
        };
    };
};