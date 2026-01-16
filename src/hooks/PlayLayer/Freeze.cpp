#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class $modify(FreezePlayLayer, PlayLayer) {
    struct Fields {
        bool enabled = options::get(key::freeze);
    };

    void updateProgressbar() {
        if (m_fields->enabled) {
            // check if current percentage is less than or equal to 90
            if (getCurrentPercentInt() >= 90) {
                auto gm = GameManager::get();

                gm->setGameVariable("0116", true);

                // Randomize FPS between 1 and 45
                int rndFps = randng::get(45, 1); // 1 to 45 inclusive

                auto interval = 1.f / static_cast<float>(rndFps);
                if (interval <= 0.f || interval > 1.f) interval = 1.f / 60.f; // fallback to 60 FPS if invalid

                CCDirector::sharedDirector()->setAnimationInterval(interval);

                // log::debug("cap fps to {} (interval {})", rndFps, interval);
            } else {
                // default to user old fps
                auto gm = GameManager::get();

                gm->setGameVariable("0116", true);

                auto oldFPS = horribleMod->getSavedValue<float>("fps");

                // Use seconds per frame, not raw FPS
                auto interval = (oldFPS > 10.f) ? (1.f / oldFPS) : (1.f / 60.f); // minimum 10 FPS
                if (interval <= 0.f || interval > 1.f) interval = 1.f / 60.f; // fallback to 60 FPS if invalid

                CCDirector::sharedDirector()->setAnimationInterval(interval);

                // log::debug("reset fps to {} (interval {})", oldFPS, interval);
            };
        };

        PlayLayer::updateProgressbar();
    };
};