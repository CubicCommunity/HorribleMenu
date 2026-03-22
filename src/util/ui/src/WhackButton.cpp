#include "../WhackButton.hpp"

#include <Utils.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static std::unordered_map<int, const char*> const s_severities = {
    {1, "diffIcon_01_btn_001.png"},
    {2, "diffIcon_02_btn_001.png"},
    {3, "diffIcon_03_btn_001.png"},
    {4, "diffIcon_04_btn_001.png"},
    {5, "diffIcon_05_btn_001.png"},
    {6, "diffIcon_07_btn_001.png"},
    {7, "diffIcon_08_btn_001.png"},
    {8, "diffIcon_06_btn_001.png"},
    {9, "diffIcon_09_btn_001.png"},
    {10, "diffIcon_10_btn_001.png"},
};

class WhackButton::Impl final {
public:
    int inputCount = 0;
    int inputTarget = s_severities.size();

    float scale = randng::get(0.875f, 0.5f);

    Button* button = nullptr;
    ProgressBar* countdown = nullptr;

    float totalTime = 5.f * randng::get(1.25f, 0.5f);
    float timeRemaining = totalTime;
    float timeDt = 0.f;

    bool success = false;
    Callback callback = nullptr;

    constexpr CircleBaseColor getButtonColor(int diff) const noexcept {
        if (diff <= 3) return CircleBaseColor::Green;
        if (diff <= 7) return CircleBaseColor::Blue;

        return CircleBaseColor::DarkPurple;
    };
};

WhackButton::WhackButton() : m_impl(std::make_unique<Impl>()) {};
WhackButton::~WhackButton() {};

void WhackButton::reload() {
    if (auto btn = WeakRef(m_impl->button).lock()) btn->removeMeAndCleanup();

    auto diff = m_impl->inputTarget - m_impl->inputCount;

    m_impl->button = Button::createWithNode(
        CircleButtonSprite::createWithSpriteFrameName(
            s_severities.at(diff),
            1.f,
            m_impl->getButtonColor(diff)),
        [self = WeakRef(this)](auto) {
            if (auto s = self.lock()) {
                s->m_impl->inputCount += 1;

                if (s->m_impl->inputCount >= s->m_impl->inputTarget) {
                    s->unscheduleUpdate();
                    s->setSuccess(true);
                } else {
                    s->reload();
                };
            };
        });
    m_impl->button->setID("whackable-btn");
    m_impl->button->setScale(m_impl->scale * 0.75f);

    setContentSize(m_impl->button->getScaledContentSize());

    m_impl->button->setPosition(getScaledContentSize() / 2.f);
    m_impl->button->setScale(0.f);

    addChild(m_impl->button, 1);

    m_impl->button->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.25f, m_impl->scale)));
};

bool WhackButton::init() {
    m_impl->inputTarget = randng::get<int>(s_severities.size(), 3);

    if (!CCNode::init()) return false;

    setID("whack-btn"_spr);
    setAnchorPoint({0.5, 0.5});

    reload();

    m_impl->countdown = ProgressBar::create(ProgressBarStyle::Solid);
    m_impl->countdown->setID("countdown");
    m_impl->countdown->setScale(0.125f * m_impl->scale);
    m_impl->countdown->setAnchorPoint({0.5, 0.5});
    m_impl->countdown->setPosition({getScaledContentWidth() / 2.f, -3.75f - (2.f * m_impl->scale)});
    m_impl->countdown->setFillColor(colors::yellow);

    addChild(m_impl->countdown, 9);

    // @geode-ignore(unknown-resource)
    playSfx("chest07.ogg");

    scheduleUpdate();

    return true;
};

void WhackButton::setCallback(Callback&& cb) {
    m_impl->callback = std::move(cb);
};

void WhackButton::callAfterFeedback(float) {
    if (m_impl->callback) m_impl->callback(m_impl->success);
};

void WhackButton::setSuccess(bool v) {
    m_impl->success = v;

    if (m_impl->button) m_impl->button->removeMeAndCleanup();

    auto symbol = CCSprite::createWithSpriteFrameName(v ? "GJ_completesIcon_001.png" : "GJ_deleteIcon_001.png");
    symbol->setID("success-icon");
    symbol->setScale(0.f);
    symbol->setPosition(getScaledContentSize() / 2.f);

    addChild(symbol, 9);
    symbol->runAction(CCSequence::createWithTwoActions(
        CCEaseSineOut::create(CCScaleTo::create(0.0875f, m_impl->scale * 2.5f)),
        CCEaseSineOut::create(CCScaleTo::create(0.125f, m_impl->scale * 1.25f))));

    // @geode-ignore(unknown-resource)
    playSfx(v ? "crystal01.ogg" : "explode_11.ogg");
    scheduleOnce(schedule_selector(WhackButton::callAfterFeedback), 1.25f);
};

void WhackButton::update(float dt) {
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

    if (m_impl->countdown) m_impl->countdown->updateProgress(pct);

    if (m_impl->timeRemaining <= 0.f) {
        setSuccess(false);
        unscheduleUpdate();
    };
};

WhackButton* WhackButton::create() {
    auto ret = new WhackButton();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};