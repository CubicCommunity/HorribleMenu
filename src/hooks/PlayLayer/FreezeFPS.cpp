#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class $modify(FreezePlayLayer, PlayLayer) {
    struct Fields {
        bool enabled = options::get(key::freeze);
        int chance = options::getChance(key::freeze);
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        auto f = m_fields.self();

        if (f->enabled) {
            if (auto gm = GameManager::sharedState()) {
                int rnd = randng::fast();
                if (rnd % 100 < f->chance) capFPS(1.f);
            };
        } else {
            log::warn("Random freezing at 90% is disabled");
        };
    };

    void pauseGame(bool p0) {
        revertFPS();
        PlayLayer::pauseGame(p0);
    };

    void revertFPS() {
        // default to user old fps
        auto gm = GameManager::get();

        float oldFPS = horribleMod->getSavedValue<float>("fps");

        gm->setGameVariable("0116", true);

        // Use seconds per frame, not raw FPS
        float interval = (oldFPS > 10.f) ? (1.f / oldFPS) : (1.f / 60.f); // minimum 10 FPS
        if (interval <= 0.f || interval > 1.f) interval = 1.f / 60.f; // fallback to 60 FPS if invalid

        CCDirector::sharedDirector()->setAnimationInterval(interval);
        log::debug("reset fps to {} (interval {})", oldFPS, interval);
    };

    void capFPS(float value) {
        auto gm = GameManager::get();

        gm->setGameVariable("0116", true);
        gm->setGameVariable("0116", true);

        float interval = 1.f / value; // cap fps to 60
        if (interval <= 0.f || interval > 1.f) interval = 1.f / 60.f; // fallback to 60 FPS if invalid

        CCDirector::sharedDirector()->setAnimationInterval(interval);
        log::debug("cap fps to {} (interval {})", value, interval);
    };

    void onQuit() {
        revertFPS();
        PlayLayer::onQuit();
    };
};