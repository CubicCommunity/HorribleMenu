#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class $modify(BlackScreenPlayLayer, PlayLayer) {
    struct Fields {
        bool enabled = options::get("black_screen");
    };

    bool init(GJGameLevel * level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        int rnd = randng::get(5);
        log::info("playlayer init called {}", rnd);

        if (m_fields->enabled) {
            // random delay between 0 and 5 seconds
            float delay = static_cast<float>(rnd);
            log::debug("Black screen will appear after {} seconds", delay);

            scheduleOnce(schedule_selector(BlackScreenPlayLayer::showBlackScreen), delay);
        };

        return true;
    };

    void showBlackScreen(float) {
        if (m_fields->enabled) {
            log::debug("Showing black screen after delay");

            auto const winSize = CCDirector::sharedDirector()->getWinSize();

            auto blackScreen = CCScale9Sprite::create("square02_001.png");
            blackScreen->setContentSize({ winSize.width + 10.f, winSize.height + 10.f });
            blackScreen->setPosition(winSize / 2.f);
            blackScreen->setID("black_screen"_spr);

            m_uiLayer->addChild(blackScreen, 99);

            // Schedule removal after 0.5 seconds, then schedule to show again after a random delay
            blackScreen->runAction(CCSequence::create(
                CCDelayTime::create(0.25f),
                CCCallFuncN::create(this, callfuncN_selector(BlackScreenPlayLayer::removeBlackScreen)),
                nullptr
            ));
        };
    };

    void removeBlackScreen(CCNode * sender) {
        if (sender) sender->removeMeAndCleanup();

        if (m_fields->enabled) {
            float delay = randng::get(3.f); // random delay between 0 and 3 seconds
            log::debug("Black screen will appear again after {} seconds", delay);

            scheduleOnce(schedule_selector(BlackScreenPlayLayer::showBlackScreen), delay);
        };
    };
};
