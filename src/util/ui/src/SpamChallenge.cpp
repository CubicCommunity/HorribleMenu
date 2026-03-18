#include "../SpamChallenge.hpp"

#include <Utils.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class SpamChallenge::Impl final {
public:
    int inputCount = 0;
    int inputTarget = 45;

    CCLabelBMFont* counter = nullptr;
    ProgressBar* timer = nullptr;

    float totalTime = 7.5f;
    float timeRemaining = totalTime;
    float timeDt = 0.f;

    bool success = false;
    Callback callback = nullptr;
};

SpamChallenge::SpamChallenge() : m_impl(std::make_unique<Impl>()) {};
SpamChallenge::~SpamChallenge() {};

bool SpamChallenge::init() {
    if (!CCBlockLayer::init()) return false;

    setID("spam-jumps"_spr);

    m_impl->inputTarget = randng::get(45, 20);

    auto const winSize = CCDirector::get()->getWinSize();

    // reuse winSize declared above
    auto label = CCLabelBMFont::create("Quick! Spam or get sent back!", "bigFont.fnt", getScaledContentWidth() - 1.25f);
    label->setID("label");
    label->setAlignment(kCCTextAlignmentCenter);
    label->setPosition({winSize.width / 2.f, winSize.height - 50.f});
    label->setScale(0.875f);

    addChild(label, 1);

    auto descLabel = CCLabelBMFont::create("Use your mouse button or tap the screen to increase the count.", "chatFont.fnt", getScaledContentWidth() - 1.25f);
    descLabel->setID("description-label");
    descLabel->setAlignment(kCCTextAlignmentCenter);
    descLabel->setPosition({winSize.width / 2.f, 25.f});
    descLabel->setAnchorPoint({0.5, 0});
    descLabel->setColor(colors::yellow);

    addChild(descLabel, 1);

    m_impl->counter = CCLabelBMFont::create(fmt::format("{} / {}", m_impl->inputCount, m_impl->inputTarget).c_str(), "goldFont.fnt");
    m_impl->counter->setID("counter");
    m_impl->counter->setScale(2.5f);
    m_impl->counter->setAlignment(kCCTextAlignmentCenter);
    m_impl->counter->setPosition({winSize.width / 2.f, (winSize.height / 2.f) - 6.25f});

    auto moveUp = CCEaseSineInOut::create(CCMoveBy::create(1.25f, ccp(0, 12.5f)));
    auto moveDown = CCEaseSineInOut::create(CCMoveBy::create(1.25f, ccp(0, -12.5f)));

    auto seq = CCSequence::createWithTwoActions(moveUp, moveDown);

    addChild(m_impl->counter, 9);
    m_impl->counter->runAction(CCRepeatForever::create(seq));

    m_impl->timer = ProgressBar::create();
    m_impl->timer->setID("timer");
    m_impl->timer->setFillColor(colors::yellow);
    m_impl->timer->setStyle(ProgressBarStyle::Solid);
    m_impl->timer->setAnchorPoint({0.5, 0.5});
    m_impl->timer->setPosition({winSize.width / 2.f, winSize.height - 20.f});

    m_impl->timer->updateProgress(100.f);

    addChild(m_impl->timer, 9);

    // @geode-ignore(unknown-resource)
    playSfx("chest07.ogg");

    scheduleUpdate();

    return true;
};

void SpamChallenge::setCallback(Callback&& cb) {
    m_impl->callback = std::move(cb);
};

bool SpamChallenge::ccTouchBegan(CCTouch* touch, CCEvent* event) {
    if (m_impl->timeRemaining > 0.f && m_impl->inputTarget > m_impl->inputCount) {
        m_impl->inputCount++;
        if (m_impl->counter) m_impl->counter->setString(fmt::format("{} / {}", m_impl->inputCount, m_impl->inputTarget).c_str());

        if (m_impl->inputCount >= m_impl->inputTarget) {
            unscheduleUpdate();
            setSuccess(true);
        };
    };

    return false;
};

void SpamChallenge::closeAfterFeedback(float) {
    if (m_impl->callback) m_impl->callback(m_impl->success);
};

void SpamChallenge::setSuccess(bool v) {
    m_impl->success = v;

    if (m_impl->counter) m_impl->counter->removeFromParentAndCleanup(false);

    auto symbol = CCSprite::createWithSpriteFrameName(v ? "GJ_completesIcon_001.png" : "GJ_deleteIcon_001.png");
    symbol->setID("success-icon");
    symbol->setScale(0.f);
    symbol->setPosition(getScaledContentSize() / 2.f);

    addChild(symbol, 9);
    symbol->runAction(CCSequence::createWithTwoActions(
        CCEaseSineOut::create(CCScaleTo::create(0.0875f, 2.75f)),
        CCEaseSineOut::create(CCScaleTo::create(0.125f, 2.5f))));

    // @geode-ignore(unknown-resource)
    playSfx(v ? "crystal01.ogg" : "explode_11.ogg");
    scheduleOnce(schedule_selector(SpamChallenge::closeAfterFeedback), 1.25f);
};

void SpamChallenge::update(float dt) {
    if (m_impl->timeRemaining <= 0.f) return;
    m_impl->timeRemaining -= dt;

    m_impl->timeDt += dt;
    if (m_impl->timeDt >= 0.5f) {
        // @geode-ignore(unknown-resource)
        playSfx("counter003.ogg");
        m_impl->timeDt = 0.f;
    };

    if (m_impl->timeRemaining < 0.f) m_impl->timeRemaining = 0.f;
    float pct = (m_impl->timeRemaining / m_impl->totalTime) * 100.f;

    if (m_impl->timer) m_impl->timer->updateProgress(pct);

    if (m_impl->timeRemaining <= 0.f) {
        setSuccess(false);
        unscheduleUpdate();
    };
};

void SpamChallenge::keyBackClicked() {
    Notification::create("You can't escape the spam challenge...", NotificationIcon::Error, 1.25f)->show();

    unscheduleUpdate();
    if (m_impl->callback) m_impl->callback(false);
};

SpamChallenge* SpamChallenge::create() {
    auto ret = new SpamChallenge();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};