#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "black_screen"

static auto const o = Option::create(THIS_ID)
                          ->setName("Black Screen Blink")
                          ->setDescription("The screen can suddenly blink while playing a level.\n<cl>suggested by elite_smiler_ispro</c>")
                          ->setCategory(category::obstructive)
                          ->setSillyTier(SillyTier::Low)
                          ->autoRegister();

class $modify(BlackScreenPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        // random delay between 0 and 5 seconds
        auto delay = rng::get(5.f);
        log::debug("Black screen will appear after {} seconds", delay);

        scheduleOnce(schedule_selector(BlackScreenPlayLayer::showBlackScreen), delay);
    };

    void showBlackScreen(float) {
        log::trace("Showing black screen after delay");

        auto const winSize = CCDirector::sharedDirector()->getWinSize();

        auto blackScreen = CCLayerColor::create();
        blackScreen->setID("blink"_spr);
        blackScreen->setColor(colors::black);
        blackScreen->setAnchorPoint(anchor::center);
        blackScreen->setContentSize(winSize);
        blackScreen->setPosition(winSize / 2.f);

        m_uiLayer->addChild(blackScreen, HIGHEST_Z);

        // Schedule removal after 0.5 seconds, then schedule to show again after a random delay
        blackScreen->runAction(CCSequence::createWithTwoActions(
            CCDelayTime::create(rng::get(0.25f, 0.125f)),
            CCCallFuncN::create(this, callfuncN_selector(BlackScreenPlayLayer::removeBlackScreen))));
    };

    void removeBlackScreen(CCNode* sender) {
        cue::resetNode(sender);

        auto delay = rng::get(2.5f);
        log::debug("Black screen will appear again after {} seconds", delay);

        scheduleOnce(schedule_selector(BlackScreenPlayLayer::showBlackScreen), delay);
    };
};
