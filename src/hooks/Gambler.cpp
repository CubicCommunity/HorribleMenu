#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "gambler"

static auto const o = Option::create(THIS_ID)
                          ->setName("Ending Gamble")
                          ->setDescription("When reaching 95% in a level, you have a chance at randomly being blasted way far back.\n<cl>suggested by Timered</c>")
                          ->setCategory(category::misc)
                          ->setSillyTier(SillyTier::Medium)
                          ->autoRegister();

class $modify(GamblerPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields {
        bool triggered = false;
        bool tricked = false;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        auto f = m_fields.self();

        log::debug("gambler enabled");

        // check every frame so we can detect each percentage change
        if (!f->triggered) schedule(schedule_selector(GamblerPlayLayer::gamblerCheck), 0.f);

        return true;
    };

    // ensure that triggered is reset on level restart/full reset
    void fullReset() {
        PlayLayer::fullReset();
        m_fields->triggered = false;
        log::debug("gambler full reset");
    };

    void resetLevel() {
        PlayLayer::resetLevel();
        m_fields->triggered = false;
        log::debug("gambler level reset");
    };

    void gamblerCheck(float) {
        auto f = m_fields.self();

        int percentage = getCurrentPercentInt();
        // detect the moment the player first reaches or crosses 95
        if (percentage == 95 && !f->triggered) {
            // roll a random number between 0 and 1
            int roll = randng::get(1);

            log::info("Gambler roll: {}", roll);
            if (roll == 0) {
                log::info("Gambler lost the bet!");

                sfx::play(sfx::file::pop);
                Notification::create("Unlucky!", NotificationIcon::Error)->show();

                // reverse the player
                m_player1->reversePlayer(nullptr);
                m_player1->m_gravity = 0.01f;  // reduce gravity to simulate a bounce

                // force player to jump
                if (auto gjbgl = GJBaseGameLayer::get()) gjbgl->handleButton(true, 1, true);

                f->triggered = true;
                f->tricked = true;
            } else {
                log::info("Gambler won the bet! instant win.");

                sfx::play(sfx::file::good);
                Notification::create("You got lucky this time...", NotificationIcon::Success)->show();

                levelComplete();
                f->triggered = true;
            };
        };
    };
};