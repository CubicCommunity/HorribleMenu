#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class $modify(ConfettiPlayLayer, PlayLayer) {
    struct Fields {
        bool enabled = options::get("confetti");

        static constexpr auto confettis = std::to_array<const char*>({
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
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        auto f = m_fields.self();

        this->template addEventListener<OptionEventFilter>(
            [this, f](OptionEvent* ev) {
                unschedule(schedule_selector(ConfettiPlayLayer::confetti));
                unschedule(schedule_selector(ConfettiPlayLayer::nextConfetti));

                f->enabled = ev->getToggled();

                if (f->enabled) scheduleOnce(schedule_selector(ConfettiPlayLayer::nextConfetti), randng::get(0.125f));

                return ListenerResult::Propagate;
            },
            "confetti"
        );

        if (f->enabled) scheduleOnce(schedule_selector(ConfettiPlayLayer::nextConfetti), randng::get(0.125f));
    };

    void nextConfetti(float) {
        auto delay = randng::get(10.f, 1.f);
        log::debug("scheduling confetti in {}s", delay);

        if (m_fields->enabled) scheduleOnce(schedule_selector(ConfettiPlayLayer::confetti), delay);
    };

    void confetti(float) {
        if (m_fields->enabled) {
            log::info("unleashing confetti!");

            playSfx("jumpscareAudio.mp3");
            shakeCamera(1.25f, 2.5f, 0.00875F);
            for (int i = 0; i < randng::get(125, 75); i++) createConfetti();
        };

        scheduleOnce(schedule_selector(ConfettiPlayLayer::nextConfetti), randng::get(0.125f));;
    };

    void createConfetti() {
        if (m_fields->enabled) {
            auto conf = CCSprite::createWithSpriteFrameName(Fields::confettis[randng::get(Fields::confettis.size() - 1)]);
            conf->setPosition({ 0.f, 0.f });
            conf->setScale(0.875f * randng::pc());

            auto useY = randng::get(1) > 0;
            auto const endPos = ccp(
                useY ? getScaledContentWidth() + conf->getScaledContentWidth() : getScaledContentWidth() * randng::pc(),
                useY ? getScaledContentHeight() * randng::pc() : getScaledContentHeight() + conf->getScaledContentHeight()
            );

            auto move = CCEaseSineOut::create(CCMoveTo::create(0.875f + randng::pc() * 2.5f, endPos));
            auto rotate = CCEaseSineOut::create(CCRotateBy::create(0.875f + randng::pc() * 2.5f, 360.f * (randng::get(1) > 0 ? 1.f : -1.f)));

            auto seq = CCSequence::createWithTwoActions(
                CCSpawn::createWithTwoActions(move, rotate),
                CCCallFuncN::create(this, callfuncN_selector(ConfettiPlayLayer::cleanConfetti))
            );

            m_uiLayer->addChild(conf, 9);
            conf->runAction(seq);
        };
    };

    void cleanConfetti(CCNode * sender) {
        if (sender) sender->removeMeAndCleanup();
    };
};