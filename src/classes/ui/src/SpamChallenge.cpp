#include "../SpamChallenge.hpp"

#include <Utils.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class SpamChallenge::Impl final {
public:
    int m_inputCount = 0;
    int m_inputTarget = 50;

    CCLabelBMFont* m_counter = nullptr;
    ProgressBar* m_timer = nullptr;

    float m_totalTime = 7.5f;
    float m_timeRemaining = m_totalTime;
    float m_timeDt = 0.f;

    bool m_success = false;
    Function<void(bool)> m_callback = nullptr;
};

SpamChallenge::SpamChallenge() : m_impl(std::make_unique<Impl>()) {};
SpamChallenge::~SpamChallenge() {};

bool SpamChallenge::init() {
    if (!CCBlockLayer::init()) return false;

    setID("spam-jumps"_spr);

    // increase spam target for mobile players
    m_impl->m_inputTarget = randng::get(50, 20) * ((GEODE_PLATFORM_TARGET & Platform::Mobile) ? 2 : 1);

    auto const winSize = CCDirector::get()->getWinSize();

    // reuse winSize declared above
    auto label = CCLabelBMFont::create("Quick! Spam or get sent back!", "bigFont.fnt", getScaledContentWidth() - 1.25f);
    label->setID("label");
    label->setAlignment(kCCTextAlignmentCenter);
    label->setPosition({ winSize.width / 2.f, winSize.height - 50.f });
    label->setScale(0.875f);

    addChild(label, 1);

    auto descLabel = CCLabelBMFont::create("Use your mouse button or tap the screen to increase the count.", "chatFont.fnt", getScaledContentWidth() - 1.25f);
    descLabel->setID("description-label");
    descLabel->setAlignment(kCCTextAlignmentCenter);
    descLabel->setPosition({ winSize.width / 2.f, 25.f });
    descLabel->setAnchorPoint({ 0.5, 0 });
    descLabel->setColor(colors::yellow);

    addChild(descLabel, 1);

    m_impl->m_counter = CCLabelBMFont::create(fmt::format("{} / {}", m_impl->m_inputCount, m_impl->m_inputTarget).c_str(), "goldFont.fnt");
    m_impl->m_counter->setID("counter");
    m_impl->m_counter->setScale(2.5f);
    m_impl->m_counter->setAlignment(kCCTextAlignmentCenter);
    m_impl->m_counter->setPosition({ winSize.width / 2.f, (winSize.height / 2.f) - 6.25f });

    auto moveUp = CCEaseSineInOut::create(CCMoveBy::create(1.25f, ccp(0, 12.5f)));
    auto moveDown = CCEaseSineInOut::create(CCMoveBy::create(1.25f, ccp(0, -12.5f)));

    auto seq = CCSequence::createWithTwoActions(moveUp, moveDown);

    addChild(m_impl->m_counter, 9);
    m_impl->m_counter->runAction(CCRepeatForever::create(seq));

    m_impl->m_timer = ProgressBar::create();
    m_impl->m_timer->setID("timer");
    m_impl->m_timer->setFillColor(colors::yellow);
    m_impl->m_timer->setStyle(ProgressBarStyle::Solid);
    m_impl->m_timer->setAnchorPoint({ 0.5, 0.5 });
    m_impl->m_timer->setPosition({ winSize.width / 2.f, winSize.height - 20.f });

    m_impl->m_timer->updateProgress(100.f);

    addChild(m_impl->m_timer, 9);

    // @geode-ignore(unknown-resource)
    playSfx("chest07.ogg");

    scheduleUpdate();

    return true;
};

void SpamChallenge::setCallback(Function<void(bool)> cb) {
    m_impl->m_callback = std::move(cb);
};

bool SpamChallenge::ccTouchBegan(CCTouch* touch, CCEvent* event) {
    if (m_impl->m_timeRemaining > 0.f && m_impl->m_inputTarget > m_impl->m_inputCount) {
        m_impl->m_inputCount++;
        if (m_impl->m_counter) m_impl->m_counter->setString(fmt::format("{} / {}", m_impl->m_inputCount, m_impl->m_inputTarget).c_str());

        if (m_impl->m_inputCount >= m_impl->m_inputTarget) {
            unscheduleUpdate();
            setSuccess(true);
        };
    };

    return false;
};

void SpamChallenge::closeAfterFeedback(float) {
    if (m_impl->m_callback) m_impl->m_callback(m_impl->m_success);
    removeFromParentAndCleanup(true);
};

void SpamChallenge::setSuccess(bool v) {
    m_impl->m_success = v;

    if (m_impl->m_counter) m_impl->m_counter->setVisible(false);

    auto symbol = CCSprite::createWithSpriteFrameName(v ? "GJ_completesIcon_001.png" : "GJ_deleteIcon_001.png");
    symbol->setID("success-icon");
    symbol->setScale(0.f);
    symbol->setPosition(getScaledContentSize() / 2.f);

    addChild(symbol, 9);
    symbol->runAction(CCSequence::createWithTwoActions(
        CCEaseSineOut::create(CCScaleTo::create(0.0875f, 2.75f)),
        CCEaseSineOut::create(CCScaleTo::create(0.125f, 2.5f))
    ));

    // @geode-ignore(unknown-resource)
    playSfx(v ? "crystal01.ogg" : "jumpscareAudio.mp3");
    scheduleOnce(schedule_selector(SpamChallenge::closeAfterFeedback), 1.25f);
};

void SpamChallenge::update(float dt) {
    if (m_impl->m_timeRemaining <= 0.f) return;
    m_impl->m_timeRemaining -= dt;

    m_impl->m_timeDt += dt;
    if (m_impl->m_timeDt >= 0.5f) {
        // @geode-ignore(unknown-resource)
        playSfx("counter003.ogg");
        m_impl->m_timeDt = 0.f;
    };

    if (m_impl->m_timeRemaining < 0.f) m_impl->m_timeRemaining = 0.f;
    float pct = (m_impl->m_timeRemaining / m_impl->m_totalTime) * 100.f;

    if (m_impl->m_timer) m_impl->m_timer->updateProgress(pct);

    if (m_impl->m_timeRemaining <= 0.f) {
        unscheduleUpdate();
        setSuccess(false);
    };
};

void SpamChallenge::keyBackClicked() {
    Notification::create("You can't escape the spam challenge...", NotificationIcon::Error, 1.25f)->show();

    unscheduleUpdate();
    if (m_impl->m_callback) m_impl->m_callback(false);
    removeFromParentAndCleanup(true);
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