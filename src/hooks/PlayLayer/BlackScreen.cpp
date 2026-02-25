#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static constexpr auto id = "black_screen";

inline static Option const o = {
    id,
    "Black Screen Blink",
    "The screen can suddenly blink while playing a level.\n<cy>Credit: elite_smiler_ispro</c>",
    category::obstructive,
    SillyTier::Low,
};
HORRIBLE_REGISTER_OPTION(o);

class $modify(BlackScreenPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(id);

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        // random delay between 0 and 5 seconds
        auto delay = randng::get(5.f);
        log::debug("Black screen will appear after {} seconds", delay);

        scheduleOnce(schedule_selector(BlackScreenPlayLayer::showBlackScreen), delay);
    };

    void showBlackScreen(float) {
        log::debug("Showing black screen after delay");

        auto const winSize = CCDirector::sharedDirector()->getWinSize();

        auto blackScreen = NineSlice::create(theme::square);
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

    void removeBlackScreen(CCNode * sender) {
        if (sender) sender->removeMeAndCleanup();

        float delay = randng::get(3.f); // random delay between 0 and 3 seconds
        log::debug("Black screen will appear again after {} seconds", delay);

        scheduleOnce(schedule_selector(BlackScreenPlayLayer::showBlackScreen), delay);
    };
};
