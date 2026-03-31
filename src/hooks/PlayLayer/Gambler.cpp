#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

static constexpr auto id = "gambler";

static auto const o = Option::create(id)
                          .setName("Gambler")
                          .setDescription("When reaching 95% in a level, you have a chance at randomly being blasted way far back.\n<cl>suggested by Timered</c>")
                          .setCategory(category::misc)
                          .setSillyTier(SillyTier::Medium);
HORRIBLE_REGISTER_OPTION(o);

class $modify(GamblerPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(id);

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
        m_fields->triggered = false;
        log::debug("gambler full reset");
        PlayLayer::fullReset();
    };

    void resetLevel() {
        m_fields->triggered = false;
        log::debug("gambler level reset");
        PlayLayer::resetLevel();
    };

    void gamblerCheck(float) {
        auto f = m_fields.self();

        int percentage = getCurrentPercentInt();
        // detect the moment the player first reaches or crosses 95
        if (percentage == 95 && !f->triggered) {
            // roll a random number between 0 and 1
            int roll = randng::fast() % 2;

            log::info("Gambler roll: {}", roll);
            if (roll == 0) {
                log::info("Gambler lost the bet!");

                // reverse the player
                m_player1->reversePlayer(nullptr);
                m_player1->m_gravity = 0.01f;  // reduce gravity to simulate a bounce

                // force player to jump
                GJBaseGameLayer::get()->handleButton(true, 1, true);

                f->triggered = true;
                f->tricked = true;
            } else {
                log::info("Gambler won the bet! instant win.");
                levelComplete();
                f->triggered = true;
            };
        };
    };
};