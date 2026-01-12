#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class $modify(FakeCrashGJBaseGameLayer, GJBaseGameLayer) {
    struct Fields {
        bool m_enabled = options::get("fake_crash");
        int m_chance = options::getChance("fake_crash");

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

        if (auto pl = PlayLayer::get()) {
            // log::debug("FakeCrash update tick");
            if (f->m_enabled && !f->m_inFakeCrash && randng::fast() % f->m_chance == 0) {
                log::debug("Faking crash");
                f->m_lastTimeWarp = LevelTools::getLastTimewarp();

                GJBaseGameLayer::updateTimeWarp(0.f);

                f->m_inFakeCrash = true;
                f->m_fakeCrashStartTime = pl->m_gameState.m_currentProgress;
            };

            if (f->m_inFakeCrash) {
                float elapsedTime = pl->m_gameState.m_currentProgress - f->m_fakeCrashStartTime;
                if (elapsedTime >= 5.f) {
                    log::debug("Reverting timewarp to: {}", f->m_lastTimeWarp);

                    GJBaseGameLayer::updateTimeWarp(f->m_lastTimeWarp);
                    f->m_inFakeCrash = false;
                };
            };
        };
    };
};