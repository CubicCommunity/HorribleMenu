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

    struct Fields final {
        bool triggered = false;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        // check every frame so we can detect each percentage change
        if (!m_fields->triggered) schedule(schedule_selector(GamblerPlayLayer::gamblerCheck), 0.f);

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

    void gamblerCheck(float) {
        auto f = m_fields.self();

        // detect the moment the player first reaches or crosses 95
        if (getCurrentPercentInt() >= 95 && !f->triggered) {
            // roll a random number between 0 and 1
            auto roll = rng::flip();

            log::info("Gambler roll: {}", roll);
            if (roll) {
                log::info("Gambler lost the bet!");

                sfx::play(sfx::file::pop);
                Notification::create("Unlucky!", NotificationIcon::Error)->show();

                // reverse the player
                m_player1->reversePlayer(nullptr);

                // force player to jump
                if (auto gjbgl = GJBaseGameLayer::get()) gjbgl->handleButton(true, 1, true);

                f->triggered = true;
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