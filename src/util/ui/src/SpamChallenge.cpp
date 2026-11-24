#include "../SpamChallenge.hpp"

#include <Util.h>

#include <rhatt/prelude.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace cs::brkd::horrible::prelude;

struct SpamChallenge::Impl final {
    uint8_t inputCount = 0;
    uint8_t inputTarget = 45;

    Label* counter = nullptr;
    ProgressBar* countdown = nullptr;

    float totalTime = 7.5f;
    float timeRemaining = totalTime;
    float timeDt = 0.0f;

    bool success = false;
    Callback callback = nullptr;
};

SpamChallenge::SpamChallenge() : m_impl(std::make_unique<Impl>()) {}
SpamChallenge::~SpamChallenge() = default;

bool SpamChallenge::init() {
    m_impl->inputTarget = cw::rng::get<uint8_t>(m_impl->inputTarget, 20);

    if (!CCBlockLayer::init()) return false;

    setID("spam-jumps"_spr);
    setKeypadEnabled(false);
    setKeyboardEnabled(false);

    auto const winSize = CCDirector::sharedDirector()->getWinSize();

    auto label = Label::create("Quick! Spam or get sent back!", cw::font::big);
    label->setID("label");
    label->setMaxWidth(getScaledContentWidth() - 1.25f);
    label->setAlignment(Label::Alignment::Center);
    label->setPosition({ winSize.width / 2.0f, winSize.height - 50.0f });
    label->setScale(0.875f);

    addChild(label, 1);

    auto descLabel = Label::createRich("Use your <cg>mouse button</c> or <cy>tap the screen</c> to increase the count.", cw::font::chat);
    descLabel->setID("description-label");
    descLabel->setMaxWidth(getScaledContentWidth() - 1.25f);
    descLabel->setAlignment(Label::Alignment::Center);
    descLabel->setPosition({ winSize.width / 2.0f, 25.0f });
    descLabel->setAnchorPoint({ 0.5, 0 });

    addChild(descLabel, 1);

    m_impl->counter = Label::create(fmt::format("{} / {}", m_impl->inputCount, m_impl->inputTarget), cw::font::gold);
    m_impl->counter->setID("counter");
    m_impl->counter->setScale(2.5f);
    m_impl->counter->setAlignment(Label::Alignment::Center);
    m_impl->counter->setPosition({ winSize.width / 2.0f, (winSize.height / 2.0f) - 6.25f });

    auto moveUp = CCEaseSineInOut::create(CCMoveBy::create(1.25f, { 0.0f, 12.5f }));
    auto moveDown = CCEaseSineInOut::create(CCMoveBy::create(1.25f, { 0.0f, -12.5f }));

    auto seq = CCSequence::createWithTwoActions(moveUp, moveDown);

    addChild(m_impl->counter, 9);
    m_impl->counter->runAction(CCRepeatForever::create(seq));

    m_impl->countdown = ProgressBar::create();
    m_impl->countdown->setID("countdown");
    m_impl->countdown->setAnchorPoint(cw::anchor::center);
    m_impl->countdown->setStyle(ProgressBar::Style::Solid);
    m_impl->countdown->setPosition({ winSize.width / 2.0f, winSize.height - 20.0f });
    m_impl->countdown->setFillColor(cw::colors::fadeColor(100.0f));

    m_impl->countdown->updateProgress(100.0f);

    addChild(m_impl->countdown, 9);

    cw::sfx::play(sfx::file::pop);

    scheduleUpdate();

    return true;
}

void SpamChallenge::setCallback(Callback&& cb) {
    m_impl->callback = std::move(cb);
}

bool SpamChallenge::ccTouchBegan(CCTouch* touch, CCEvent* event) {
    if (m_impl->timeRemaining > 0.0f && m_impl->inputTarget > m_impl->inputCount) {
        m_impl->inputCount++;

        if (m_impl->counter) m_impl->counter->setText(fmt::format("{} / {}", m_impl->inputCount, m_impl->inputTarget));
        if (m_impl->inputCount >= m_impl->inputTarget) setSuccess(true);
    };

    return false;
}

void SpamChallenge::callAfterFeedback(float) {
    if (m_impl->callback) m_impl->callback(m_impl->success);
    unscheduleAllSelectors();
}

void SpamChallenge::setSuccess(bool v) {
    m_impl->success = v;

    unscheduleUpdate();

    cue::resetNode(m_impl->counter);

    auto symbol = CCSprite::createWithSpriteFrameName(m_impl->success ? "GJ_completesIcon_001.png" : "GJ_deleteIcon_001.png");
    symbol->setID("success-icon");
    symbol->setScale(0.0f);
    symbol->setPosition(getScaledContentSize() / 2.0f);

    addChild(symbol, 9);
    symbol->runAction(CCSequence::createWithTwoActions(CCEaseSineOut::create(CCScaleTo::create(0.0875f, 2.75f)), CCEaseSineOut::create(CCScaleTo::create(0.125f, 2.5f))));

    cw::sfx::play(m_impl->success ? sfx::file::good : sfx::file::bad);
    scheduleOnce(schedule_selector(SpamChallenge::callAfterFeedback), 1.25f);
}

void SpamChallenge::update(float dt) {
    if (m_impl->timeRemaining <= 0.0f) return unscheduleUpdate();
    m_impl->timeRemaining -= dt;

    m_impl->timeDt += dt;
    if (m_impl->timeDt >= 0.5f) {
        cw::sfx::play(sfx::file::count);
        m_impl->timeDt = 0.0f;
    };

    if (m_impl->timeRemaining < 0.0f) m_impl->timeRemaining = 0.0f;
    auto pct = (m_impl->timeRemaining / m_impl->totalTime) * 100.0f;

    if (m_impl->countdown) {
        m_impl->countdown->updateProgress(pct);
        m_impl->countdown->setFillColor(cw::colors::fadeColor(pct));
    };

    if (m_impl->timeRemaining <= 0.0f) {
        setSuccess(false);
        unscheduleUpdate();
    };
}

SpamChallenge* SpamChallenge::create() {
    auto ret = new SpamChallenge();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
}