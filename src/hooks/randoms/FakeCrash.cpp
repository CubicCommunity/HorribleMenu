#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "fake_crash"

static auto const o = Option::create(THIS_ID)
                          ->setName("Random Fake Crash")
                          ->setDescription("While playing a level, there's a chance your game will fake a crash.\n<cl>suggested by Timered</c>")
                          ->setCategory(category::randoms)
                          ->setSillyTier(SillyTier::Medium)
                          ->autoRegister();

class $modify(FakeCrashGJBaseGameLayer, GJBaseGameLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields {
        unsigned int chance = options::getChance(THIS_ID);

        float lastTimeWarp = LevelTools::getLastTimewarp();

        bool inFakeCrash = false;
        float fakeCrashStartTime = 0.f;
    };

    bool init() {
        if (!GJBaseGameLayer::init()) return false;

        schedule(schedule_selector(FakeCrashGJBaseGameLayer::scheduler), 0.125f);

        return true;
    };

    void scheduler(float) {
        auto f = m_fields.self();

        if (!f->inFakeCrash && randng::fast() % f->chance == 0) {
            log::debug("Faking crash");
            f->lastTimeWarp = LevelTools::getLastTimewarp();

            updateTimeWarp(0.f);

            f->inFakeCrash = true;
            f->fakeCrashStartTime = m_gameState.m_currentProgress;
        };

        if (f->inFakeCrash) {
            if ((m_gameState.m_currentProgress - f->fakeCrashStartTime) >= 5.f) {
                log::debug("Reverting timewarp to: {}", f->lastTimeWarp);

                updateTimeWarp(f->lastTimeWarp);
                f->inFakeCrash = false;
            };
        };
    };
};