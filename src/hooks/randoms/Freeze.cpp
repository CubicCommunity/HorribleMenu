#include <Util.h>

#include <rhatt/prelude.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace cs::brkd::horrible::prelude;

#define THIS_ID "freeze"

static auto const o = Option::create(THIS_ID)
                          ->setName("Random 90%+ FPS Drop")
                          ->setDescription("Your visual framerate starts randomly dropping during 90-99% in a level.\n<cl>suggested by Hexfire</c>")
                          ->setCategory(category::randoms)
                          ->setSillyTier(SillyTier::Medium)
                          ->setCheating(true)
                          ->autoRegister();

class $modify(FreezeMenuLayer, MenuLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    bool init() {
        if (!MenuLayer::init()) return false;

        if (auto gm = GameManager::sharedState()) {
            // get and store user current fps
            float currentFPS = gm->m_customFPSTarget;
            (void)mod->setSavedValue<float>("fps", currentFPS);

            log::debug("Stored current FPS: {}", currentFPS);
        };

        return true;
    }
};

class $modify(FreezePlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        uint8_t chance = options::getChance(THIS_ID);
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        auto f = m_fields.self();

        if (auto gm = GameManager::sharedState()) {
            if (cw::rng::fast() < f->chance) capFPS(1.0f);
        };
    }

    void pauseGame(bool p0) {
        revertFPS();
        PlayLayer::pauseGame(p0);
    }

    void revertFPS() {
        // default to user old fps
        auto gm = GameManager::sharedState();

        float oldFPS = mod->getSavedValue<float>("fps");

        gm->setGameVariable("0116", true);

        // Use seconds per frame, not raw FPS
        float interval = (oldFPS > 10.0f) ? (1.0f / oldFPS) : (1.0f / 60.0f);  // minimum 10 FPS
        if (interval <= 0.0f || interval > 1.0f) interval = 1.0f / 60.0f;      // fallback to 60 FPS if invalid

        CCDirector::sharedDirector()->setAnimationInterval(interval);
        log::debug("reset fps to {} (interval {})", oldFPS, interval);
    }

    void capFPS(float value) {
        auto gm = GameManager::sharedState();

        gm->setGameVariable("0116", true);
        gm->setGameVariable("0116", true);

        float interval = 1.0f / value;                                     // cap fps to 60
        if (interval <= 0.0f || interval > 1.0f) interval = 1.0f / 60.0f;  // fallback to 60 FPS if invalid

        CCDirector::sharedDirector()->setAnimationInterval(interval);
        log::debug("cap fps to {} (interval {})", value, interval);
    }

    void onQuit() {
        revertFPS();
        PlayLayer::onQuit();
    }

    void updateProgressbar() {
        // check if current percentage is less than or equal to 90
        if (getCurrentPercentInt() >= 90) {
            auto gm = GameManager::sharedState();

            gm->setGameVariable("0116", true);

            // Randomize FPS between 1 and 45
            int rndFps = cw::rng::get(45, 1);  // 1 to 45 inclusive

            auto interval = 1.0f / static_cast<float>(rndFps);
            if (interval <= 0.0f || interval > 1.0f) interval = 1.0f / 60.0f;  // fallback to 60 FPS if invalid

            CCDirector::sharedDirector()->setAnimationInterval(interval);

            // log::debug("cap fps to {} (interval {})", rndFps, interval);
        } else {
            // default to user old fps
            auto gm = GameManager::sharedState();

            gm->setGameVariable("0116", true);

            auto oldFPS = mod->getSavedValue<float>("fps");

            // Use seconds per frame, not raw FPS
            auto interval = (oldFPS > 10.0f) ? (1.0f / oldFPS) : (1.0f / 60.0f);  // minimum 10 FPS
            if (interval <= 0.0f || interval > 1.0f) interval = 1.0f / 60.0f;     // fallback to 60 FPS if invalid

            CCDirector::sharedDirector()->setAnimationInterval(interval);

            // log::debug("reset fps to {} (interval {})", oldFPS, interval);
        };

        PlayLayer::updateProgressbar();
    }
};