#include "../Captcha.hpp"

#include <Utils.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

// id, sprite name
static constexpr auto s_buttons = std::to_array<std::pair<const char*, const char*>>({
    {"", ""},
});

class Captcha::Impl final {
public:
    std::string correctID = "";

    RobotVerifier* verifier = nullptr;

    ProgressBar* countdown = nullptr;

    float totalTime = 10.f;
    float timeRemaining = totalTime;
    float timeDt = 0.f;

    bool success = false;
    Callback callback = nullptr;
};

Captcha::Captcha() : m_impl(std::make_unique<Impl>()) {};
Captcha::~Captcha() {};

bool Captcha::init() {
    auto const theme = mod->getSettingValue<std::string>("theme");

    if (!Popup::init({375.f, 250.f}, themes::getBackgroundSprite(theme))) return false;

    setID("captcha"_spr);
    setTitle("Woah there!");
    setKeypadEnabled(false);
    setKeyboardEnabled(false);
    setCloseButtonSpr(CircleButtonSprite::createWithSpriteFrameName(themes::close, 0.875f, themes::getCircleBaseColor(theme)));

    popup::closeBtnID(m_closeBtn);

    m_closeBtn->setVisible(false);
    m_closeBtn->setEnabled(false);

    auto label = CCLabelBMFont::create("You're playing almost too well... Are you sure you're not a robot?", "chatFont.fnt");
    label->setID("message");
    label->setScale(0.875f);
    label->setAlignment(kCCTextAlignmentCenter);
    label->setPosition({m_mainLayer->getScaledContentWidth() / 2.f, m_mainLayer->getScaledContentHeight() - 37.5f});
    label->setAnchorPoint(anchor::center);

    m_mainLayer->addChild(label);

    m_impl->countdown = ProgressBar::create(ProgressBarStyle::Solid);
    m_impl->countdown->setID("countdown");
    m_impl->countdown->setScale(0.625f);
    m_impl->countdown->setFillColor(colors::yellow);
    m_impl->countdown->setAnchorPoint(anchor::center);
    m_impl->countdown->setPosition({m_mainLayer->getScaledContentWidth() / 2.f, 12.5f});

    m_impl->countdown->updateProgress(100.f);

    m_mainLayer->addChild(m_impl->countdown);

    scheduleUpdate();

    sfx::play(sfx::file::pop);

    return true;
};

void Captcha::setCallback(Callback&& cb) {
    m_impl->callback = std::move(cb);
};

void Captcha::callAfterFeedback(float) {
    if (m_impl->callback) m_impl->callback(m_impl->success);
    unscheduleAllSelectors();
};

void Captcha::setSuccess(bool v) {
    m_impl->success = v;

    auto symbol = CCSprite::createWithSpriteFrameName(m_impl->success ? "GJ_completesIcon_001.png" : "GJ_deleteIcon_001.png");
    symbol->setID("success-icon");
    symbol->setScale(0.f);
    symbol->setPosition(m_mainLayer->getScaledContentSize() / 2.f);

    m_mainLayer->addChild(symbol, 9);

    symbol->runAction(CCSequence::createWithTwoActions(
        CCEaseSineOut::create(CCScaleTo::create(0.0875f, 2.75f)),
        CCEaseSineOut::create(CCScaleTo::create(0.125f, 2.5f))));

    sfx::play(m_impl->success ? sfx::file::good : sfx::file::bad);
    scheduleOnce(schedule_selector(Captcha::callAfterFeedback), 1.25f);
};

void Captcha::update(float dt) {
    if (m_impl->timeRemaining <= 0.f) return unscheduleUpdate();
    m_impl->timeRemaining -= dt;

    m_impl->timeDt += dt;
    if (m_impl->timeDt >= 0.5f) {
        // @geode-ignore(unknown-resource)
        sfx::play("counter003.ogg");
        m_impl->timeDt = 0.f;
    };

    if (m_impl->timeRemaining < 0.f) m_impl->timeRemaining = 0.f;
    auto pct = (m_impl->timeRemaining / m_impl->totalTime) * 100.f;

    if (m_impl->countdown) m_impl->countdown->updateProgress(pct);

    if (m_impl->timeRemaining <= 0.f) {
        setSuccess(false);
        unscheduleUpdate();
    };
};

Captcha* Captcha::create() {
    auto ret = new Captcha();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};

bool RobotVerifier::init(std::string id, Callback&& cb) {
    m_correctID = std::move(id);
    m_callback = std::move(cb);

    if (!CCNode::init()) return false;

    auto layout = RowLayout::create()
                      ->setGap(2.5f)
                      ->setGrowCrossAxis(true);

    setID("captcha-verifier");
    setAnchorPoint(anchor::center);
    setLayout(layout);

    for (int i = 0; i < 9; ++i) {
        auto btnData = s_buttons[rng::get(s_buttons.size() - 1)];

        auto btn = Button::createWithSpriteFrameName(
            btnData.second,
            [this, id = btnData.first](auto) {
                if (id == m_correctID) {
                    m_callback(true);
                } else {
                    m_callback(false);
                };
            });
        btn->setID(btnData.first);

        cue::rescaleToMatch(btn, 25.f);
    };

    return true;
};

RobotVerifier* RobotVerifier::create(std::string id, Callback&& cb) {
    auto ret = new RobotVerifier();
    if (ret->init(std::move(id), std::move(cb))) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};