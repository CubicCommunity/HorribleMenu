#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "confetti"

static auto const o = Option::create(THIS_ID)
                          ->setName("Confetti Explosions")
                          ->setDescription("While playing a level, the screen will sometimes cause an explosion of random textures.\n<cl>created by Cheeseworks</c>")
                          ->setCategory(category::obstructive)
                          ->setSillyTier(SillyTier::Medium)
                          ->autoRegister();

static constexpr auto s_confettis = std::to_array<const char*>({
    "diffIcon_02_btn_001.png",
    "explosionIcon_20_001.png",
    "GJ_duplicateObjectBtn2_001.png",
    "diffIcon_10_btn_001.png",
    "modBadge_01_001.png",
    "miniSkull_001.png",
    "secretCoinUI_001.png",
    "secretCoinUI2_001.png",
    "GJ_rewardBtn_001.png",
    "GJ_achImage_001.png",
    "GJ_likesIcon_001.png",
    "btn_chatHistory_001.png",
    "GJ_starsIcon_001.png",
    "GJ_sMagicIcon_001.png",
    "GJ_pointsIcon_001.png",
});

class $modify(ConfettiPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        scheduleOnce(schedule_selector(ConfettiPlayLayer::nextConfetti), rng::get(0.125f));
    };

    void nextConfetti(float) {
        auto delay = rng::get(8.75f, 1.25f);
        log::trace("scheduling confetti in {}s", delay);

        scheduleOnce(schedule_selector(ConfettiPlayLayer::confetti), delay);
    };

    void confetti(float) {
        log::info("unleashing confetti!");

        sfx::play(sfx::file::bad);
        shakeCamera(0.875f, 2.5f, 0.00875f);

        for (int i = 0; i < rng::get(150, 75); i++) createConfetti();

        scheduleOnce(schedule_selector(ConfettiPlayLayer::nextConfetti), rng::get(0.125f));
    };

    void createConfetti() {
        auto conf = CCSprite::createWithSpriteFrameName(s_confettis[rng::get(s_confettis.size() - 1)]);
        conf->setScale(1.25f * rng::pc());

        auto useY = rng::flip();
        auto const endPos = ccp(
            useY ? getScaledContentWidth() + conf->getScaledContentWidth() : getScaledContentWidth() * rng::pc(),
            useY ? getScaledContentHeight() * rng::pc() : getScaledContentHeight() + conf->getScaledContentHeight());

        auto dur = 0.875f + rng::pc() * 2.5f;

        auto move = CCEaseSineOut::create(CCMoveTo::create(dur, endPos));
        auto rotate = CCEaseSineOut::create(CCRotateBy::create(dur * rng::get(2.5f, 1.f), (360.f * (rng::flip() ? 1.f : -1.f)) * rng::get(3.75f, 0.875f)));

        auto seq = CCSequence::createWithTwoActions(
            CCSpawn::createWithTwoActions(move, rotate),
            CCCallFuncN::create(this, callfuncN_selector(ConfettiPlayLayer::cleanConfetti)));

        m_uiLayer->addChild(conf, 9);
        conf->runAction(seq);
    };

    void cleanConfetti(CCNode* sender) {
        cue::resetNode(sender);
    };
};