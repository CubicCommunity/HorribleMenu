#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class $modify(BlackScreenPlayLayer, PlayLayer) {
    struct Fields {
        bool enabled = options::get(key::black_screen);
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        if (m_fields->enabled) {
            // random delay between 0 and 5 seconds
            auto delay = randng::get(5.f);
            log::debug("Black screen will appear after {} seconds", delay);

            scheduleOnce(schedule_selector(BlackScreenPlayLayer::showBlackScreen), delay);
        };
    };

    void showBlackScreen(float) {
        if (m_fields->enabled) {
            log::debug("Showing black screen after delay");

            auto const winSize = CCDirector::sharedDirector()->getWinSize();

            auto blackScreen = CCScale9Sprite::create("square02_001.png");
            blackScreen->setID("blink"_spr);
            blackScreen->setContentSize({ winSize.width + 10.f, winSize.height + 10.f });
            blackScreen->setPosition(winSize / 2.f);

            m_uiLayer->addChild(blackScreen, 99);

            // Schedule removal after 0.5 seconds, then schedule to show again after a random delay
            blackScreen->runAction(CCSequence::createWithTwoActions(
                CCDelayTime::create(0.25f),
                CCCallFuncN::create(this, callfuncN_selector(BlackScreenPlayLayer::removeBlackScreen))
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
