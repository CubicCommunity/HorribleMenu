#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

static constexpr auto id = "sleepy";

static auto const o = Option::create(id)
                          .setName("Sleepy Player")
                          .setDescription("Your character will occasionally fall asleep while playing.\n<cl>Credit: this_guy_yt</c>")
                          .setCategory(category::misc)
                          .setSillyTier(SillyTier::Low);
HORRIBLE_REGISTER_OPTION(o);

class $modify(SleepyPlayerObject, PlayerObject) {
    HORRIBLE_DELEGATE_HOOKS(id);

    struct Fields {
        unsigned int chance = options::getChance(id);

        bool m_sleepy = false;  // decelerating-to-zero stage
        bool m_waking = false;  // 5s buffer stage (cannot be re-slept)

        float m_defSpeed = 0.f;  // original speed captured at sleep start
    };

    bool init(int player, int ship, GJBaseGameLayer* gameLayer, CCLayer* layer, bool playLayer) {
        if (!PlayerObject::init(player, ship, gameLayer, layer, playLayer)) return false;

        auto f = m_fields.self();

        if (playLayer) scheduleOnce(schedule_selector(SleepyPlayerObject::asleep), randng::get(30.f, 5.f) * chanceToDelayPct(f->chance));

        return true;
    };

    void startSleepTimer(float) {
        auto f = m_fields.self();
        scheduleOnce(schedule_selector(SleepyPlayerObject::wakeUpSchedule), randng::get(15.f, 3.f) * chanceToDelayPct(f->chance));
    };

    void wakeUpSchedule(float) {
        log::debug("Waking the player up");

        auto f = m_fields.self();

        f->m_sleepy = false;
        f->m_waking = true;

        m_playerSpeed = f->m_defSpeed;  // snap back to original speed

        scheduleOnce(schedule_selector(SleepyPlayerObject::fullyWakeUpSchedule), 5.f);
    };

    // schedule wrapper for fullyWakeUp
    void fullyWakeUpSchedule(float) {
        m_fields->m_waking = false;
    };

    bool pushButton(PlayerButton p0) {
        // no jumping while m_sleepy
        if (m_fields->m_sleepy) return false;
        return PlayerObject::pushButton(p0);
    };

    void asleep(float) {
        auto f = m_fields.self();

        // player sleepy if not already in any stage
        auto onGround = m_isOnGround || m_isOnGround2 || m_isOnGround3 || m_isOnGround4;
        if (!f->m_sleepy && !f->m_waking && onGround) {
            log::debug("Making the player sleepy");

            f->m_defSpeed = m_playerSpeed;  // capture original speed
            f->m_sleepy = true;

            scheduleOnce(schedule_selector(SleepyPlayerObject::startSleepTimer), 0.25f);

            // go to sleep, go to sleep, sweet little baby go to sleep
            schedule(schedule_selector(SleepyPlayerObject::fallAsleep), 0.125f);
        };
    };

    void fallAsleep(float dt) {
        auto f = m_fields.self();

        // decelerate to 0 speed
        if (f->m_sleepy) {
            auto decelRate = f->m_defSpeed / 5.f;  // decel to 0 in 5s
            m_playerSpeed -= decelRate * dt;

            if (m_playerSpeed <= 0.f) {
                m_playerSpeed = 0.f;
                unschedule(schedule_selector(SleepyPlayerObject::fallAsleep));
            };
        };
    };
};