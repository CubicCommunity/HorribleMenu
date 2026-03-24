#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

static constexpr auto id = "freeze";

static auto const o = Option::create(id)
                          .setName("Random 90%+ FPS Drop")
                          .setDescription("Your visual framerate starts randomly dropping during 90-99% in a level.\n<cl>Credit: Hexfire</c>")
                          .setCategory(category::randoms)
                          .setSillyTier(SillyTier::Medium);
HORRIBLE_REGISTER_OPTION(o);

class $modify(FreezeMenuLayer, MenuLayer) {
    HORRIBLE_DELEGATE_HOOKS(id);

    bool init() {
        if (!MenuLayer::init()) return false;

        if (auto gm = GameManager::get()) {
            // get and store user current fps
            float currentFPS = gm->m_customFPSTarget;
            (void)thisMod->setSavedValue<float>("fps", currentFPS);

            log::debug("Stored current FPS: {}", currentFPS);
        };

        return true;
    };
};

class $modify(FreezePlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(id);

    struct Fields {
        unsigned int chance = options::getChance(id);
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        auto f = m_fields.self();

        if (auto gm = GameManager::sharedState()) {
            int rnd = randng::fast();
            if (rnd % 100 < f->chance) capFPS(1.f);
        };
    };

    void pauseGame(bool p0) {
        revertFPS();
        PlayLayer::pauseGame(p0);
    };

    void revertFPS() {
        // default to user old fps
        auto gm = GameManager::get();

        float oldFPS = thisMod->getSavedValue<float>("fps");

        gm->setGameVariable("0116", true);

        // Use seconds per frame, not raw FPS
        float interval = (oldFPS > 10.f) ? (1.f / oldFPS) : (1.f / 60.f);  // minimum 10 FPS
        if (interval <= 0.f || interval > 1.f) interval = 1.f / 60.f;      // fallback to 60 FPS if invalid

        CCDirector::sharedDirector()->setAnimationInterval(interval);
        log::debug("reset fps to {} (interval {})", oldFPS, interval);
    };

    void capFPS(float value) {
        auto gm = GameManager::get();

        gm->setGameVariable("0116", true);
        gm->setGameVariable("0116", true);

        float interval = 1.f / value;                                  // cap fps to 60
        if (interval <= 0.f || interval > 1.f) interval = 1.f / 60.f;  // fallback to 60 FPS if invalid

        CCDirector::sharedDirector()->setAnimationInterval(interval);
        log::debug("cap fps to {} (interval {})", value, interval);
    };

    void onQuit() {
        revertFPS();
        PlayLayer::onQuit();
    };

    void updateProgressbar() {
        // check if current percentage is less than or equal to 90
        if (getCurrentPercentInt() >= 90) {
            auto gm = GameManager::get();

            gm->setGameVariable("0116", true);

            // Randomize FPS between 1 and 45
            int rndFps = randng::get(45, 1);  // 1 to 45 inclusive

            auto interval = 1.f / static_cast<float>(rndFps);
            if (interval <= 0.f || interval > 1.f) interval = 1.f / 60.f;  // fallback to 60 FPS if invalid

            CCDirector::sharedDirector()->setAnimationInterval(interval);

            // log::debug("cap fps to {} (interval {})", rndFps, interval);
        } else {
            // default to user old fps
            auto gm = GameManager::get();

            gm->setGameVariable("0116", true);

            auto oldFPS = thisMod->getSavedValue<float>("fps");

            // Use seconds per frame, not raw FPS
            auto interval = (oldFPS > 10.f) ? (1.f / oldFPS) : (1.f / 60.f);  // minimum 10 FPS
            if (interval <= 0.f || interval > 1.f) interval = 1.f / 60.f;     // fallback to 60 FPS if invalid

            CCDirector::sharedDirector()->setAnimationInterval(interval);

            // log::debug("reset fps to {} (interval {})", oldFPS, interval);
        };

        PlayLayer::updateProgressbar();
    };
};