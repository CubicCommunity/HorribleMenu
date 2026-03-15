#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static constexpr auto id = "confetti";

inline static Option const o = {
    id,
    "Confetti Explosion",
    "While playing a level, the screen will sometimes cause an explosion of random textures.\n<cl>Credit: Cheeseworks</c>",
    category::obstructive,
    SillyTier::Medium,
};
HORRIBLE_REGISTER_OPTION(o);

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
    HORRIBLE_DELEGATE_HOOKS(id);

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        scheduleOnce(schedule_selector(ConfettiPlayLayer::nextConfetti), randng::get(0.125f));
    };

    void nextConfetti(float) {
        auto delay = randng::get(10.f, 1.f);
        log::debug("scheduling confetti in {}s", delay);

        scheduleOnce(schedule_selector(ConfettiPlayLayer::confetti), delay);
    };

    void confetti(float) {
        log::info("unleashing confetti!");

        playSfx("explode_11.ogg");
        shakeCamera(1.25f, 2.5f, 0.00875F);
        for (int i = 0; i < randng::get(125, 75); i++)
            createConfetti();

        scheduleOnce(schedule_selector(ConfettiPlayLayer::nextConfetti), randng::get(0.125f));
        ;
    };

    void createConfetti() {
        auto conf = CCSprite::createWithSpriteFrameName(s_confettis[randng::get(s_confettis.size() - 1)]);
        conf->setPosition({0.f, 0.f});
        conf->setScale(0.875f * randng::pc());

        auto useY = randng::get(1) > 0;
        auto const endPos = ccp(
            useY ? getScaledContentWidth() + conf->getScaledContentWidth() : getScaledContentWidth() * randng::pc(),
            useY ? getScaledContentHeight() * randng::pc() : getScaledContentHeight() + conf->getScaledContentHeight());

        auto move = CCEaseSineOut::create(CCMoveTo::create(0.875f + randng::pc() * 2.5f, endPos));
        auto rotate = CCEaseSineOut::create(CCRotateBy::create(0.875f + randng::pc() * 2.5f, 360.f * (randng::get(1) > 0 ? 1.f : -1.f)));

        auto seq = CCSequence::createWithTwoActions(
            CCSpawn::createWithTwoActions(move, rotate),
            CCCallFuncN::create(this, callfuncN_selector(ConfettiPlayLayer::cleanConfetti)));

        m_uiLayer->addChild(conf, 9);
        conf->runAction(seq);
    };

    void cleanConfetti(CCNode* sender) {
        if (sender) sender->removeMeAndCleanup();
    };
};