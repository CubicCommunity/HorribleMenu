#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static Option const o = {
    "fake_crash",
    "Random Fake Crash",
    "While playing a level, there's a chance your game will fake a crash.\n<cy>Credit: Timered</c>",
    category::randoms,
    SillyTier::Medium,
};
REGISTER_HORRIBLE_OPTION(o);

class $modify(FakeCrashGJBaseGameLayer, GJBaseGameLayer) {
    struct Fields {
        bool m_enabled = options::get(o.id);
        int m_chance = options::getChance(o.id);

        float m_lastTimeWarp = LevelTools::getLastTimewarp();
        bool m_inFakeCrash = false;
        float m_fakeCrashStartTime = 0.f;
    };

    bool init() {
        if (!GJBaseGameLayer::init()) return false;

        schedule(schedule_selector(FakeCrashGJBaseGameLayer::scheduler), 0.125f);

        return true;
    };

    void scheduler(float) {
        auto f = m_fields.self();

        // log::debug("FakeCrash update tick");
        if (f->m_enabled && !f->m_inFakeCrash && randng::fast() % f->m_chance == 0) {
            log::debug("Faking crash");
            f->m_lastTimeWarp = LevelTools::getLastTimewarp();

            GJBaseGameLayer::updateTimeWarp(0.f);

            f->m_inFakeCrash = true;
            f->m_fakeCrashStartTime = m_gameState.m_currentProgress;
        };

        if (f->m_inFakeCrash) {
            if ((m_gameState.m_currentProgress - f->m_fakeCrashStartTime) >= 5.f) {
                log::debug("Reverting timewarp to: {}", f->m_lastTimeWarp);

                GJBaseGameLayer::updateTimeWarp(f->m_lastTimeWarp);
                f->m_inFakeCrash = false;
            };
        };
    };
};