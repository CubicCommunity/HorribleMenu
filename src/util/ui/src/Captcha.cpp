#include "../Captcha.hpp"

#include <Util.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

// id, sprite name
static constexpr auto g_buttons = std::to_array<std::pair<const char*, const char*>>({
    {"The Yellow One", "icon_yellow.png"_spr},
    {"Furry", "icon_colonthree.png"_spr},
    {"Extreme David", "diffIcon_10_btn_001.png"},
    {"Money", "currencyOrbIcon_001.png"},
    {"Cooler Vaultkeeper", "GJ_rateDiffBtnMod_001.png"},
    {"Compact Lists", "GJ_smallModeIcon_001.png"},
    {"Subscribe to Breakeode", "gj_ytIcon_001.png"},
    {"Globed Death Effect", "explosionIcon_20_001.png"},
});

struct Captcha::Impl final {
    std::string expected = "";

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

void Captcha::setupVerifier(std::string btnID) {
    cue::resetNode(m_impl->verifier);

    m_impl->verifier = RobotVerifier::create(std::move(btnID), [this](bool success) {
        setSuccess(success);
    });
    m_impl->verifier->setPosition({m_mainLayer->getScaledContentWidth() / 2.f, (m_mainLayer->getScaledContentHeight() / 2.f) - 8.75f});

    m_mainLayer->addChild(m_impl->verifier, 9);
};

bool Captcha::init() {
    m_impl->expected = g_buttons[rng::get(g_buttons.size() - 1)].first;

    auto const theme = mod->getSettingValue<std::string>("theme");

    if (!Popup::init({400.f, 275.f}, themes::getBackgroundSprite(theme))) return false;

    setID("captcha"_spr);
    setTitle("Woah there!");
    setKeypadEnabled(false);
    setKeyboardEnabled(false);
    setCloseButtonSpr(themes::createThemeCircleSprite(themes::getCircleBaseColor(theme)));

    popup::closeBtnID(m_closeBtn);

    m_closeBtn->setVisible(false);
    m_closeBtn->setEnabled(false);

    m_bgSprite->setZOrder(-9);

    auto label = Label::create("You're playing almost too well... Are you sure you're not a robot?", "chatFont.fnt");
    label->setID("message");
    label->setScale(0.75f);
    label->setAlignment(Label::Alignment::Center);
    label->setPosition({m_mainLayer->getScaledContentWidth() / 2.f, m_mainLayer->getScaledContentHeight() - 37.5f});
    label->setAnchorPoint(anchor::center);

    m_mainLayer->addChild(label);

    m_impl->countdown = ProgressBar::create(ProgressBarStyle::Solid);
    m_impl->countdown->setID("countdown");
    m_impl->countdown->setScale(0.625f);
    m_impl->countdown->setAnchorPoint(anchor::center);
    m_impl->countdown->setPosition({m_mainLayer->getScaledContentWidth() / 2.f, 17.5f});
    m_impl->countdown->setFillColor(colors::fadeColor(100.f));

    m_impl->countdown->updateProgress(100.f);

    m_mainLayer->addChild(m_impl->countdown);

    setupVerifier(m_impl->expected);

    auto bg = cue::createBackground(
        {
            m_impl->verifier->getScaledContentWidth() * 1.25f,
            192.5f,
        },
        {
            .zOrder = -1,
        });
    bg->setPosition(m_impl->verifier->getPosition());

    m_mainLayer->addChild(bg);

    auto hintID = Label::create(m_impl->expected.c_str(), "bigFont.fnt");
    hintID->setID("hint-id");
    hintID->setScale(0.5f);
    hintID->setAnchorPoint(anchor::center);
    hintID->setAlignment(Label::Alignment::Center);
    hintID->setLimitLabelWidth(bg->getScaledContentWidth() * 0.875f, 0.5f, 0.125f);
    hintID->setPosition({m_impl->verifier->getPositionX(), m_impl->verifier->getPositionY() + (m_impl->verifier->getScaledContentHeight() / 2.f) + 12.5f});

    m_mainLayer->addChild(hintID, 1);

    auto hint = SimpleTextArea::create("Press all the buttons with", "chatFont.fnt", 0.5f, bg->getScaledContentWidth() * 0.875f);
    hint->setID("hint");
    hint->setColor(to4B(colors::yellow));
    hint->setAlignment(kCCTextAlignmentCenter);
    hint->setPosition({m_impl->verifier->getPositionX(), hintID->getPositionY() + 10.f});

    m_mainLayer->addChild(hint);

    auto refreshBtn = Button::createWithNode(
        ButtonSprite::create(
            "Refresh",
            "bigFont.fnt",
            themes::getButtonSquareSprite(theme)),
        [this](auto) {
            setupVerifier(m_impl->expected);
        });
    refreshBtn->setID("refresh-btn");
    refreshBtn->setScale(0.625f);
    refreshBtn->setPosition({m_impl->verifier->getPositionX(), m_impl->verifier->getPositionY() - (m_impl->verifier->getScaledContentHeight() / 2.f) - (refreshBtn->getScaledContentHeight() * 0.825f)});

    m_mainLayer->addChild(refreshBtn, 1);

    auto infoBtn = Button::createWithSpriteFrameName(
        "GJ_infoIcon_001.png",
        [this](auto) {
            unscheduleUpdate();

            createQuickPopup(
                "Help",
                "Press on the images that correspond to the provided captcha hint. It is purposefully obscured to make this sillier.\n\n<cc>Refresh the captcha if you can't find any button that matches the hint.</c>",
                "OK",
                nullptr,  // captcha popup can exit asynchronously
                [self = WeakRef(this)](auto, auto) {
                    if (auto s = self.lock()) s->scheduleUpdate();
                });
        });
    infoBtn->setID("info-btn");
    infoBtn->setScale(0.875f);

    m_mainLayer->addChildAtPosition(infoBtn, Anchor::TopRight, {-17.5f, -17.5f});

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

    unscheduleUpdate();

    cue::resetNode(m_impl->verifier);

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
        sfx::play(sfx::file::count);
        m_impl->timeDt = 0.f;
    };

    if (m_impl->timeRemaining < 0.f) m_impl->timeRemaining = 0.f;
    auto pct = (m_impl->timeRemaining / m_impl->totalTime) * 100.f;

    if (m_impl->countdown) {
        m_impl->countdown->updateProgress(pct);
        m_impl->countdown->setFillColor(colors::fadeColor(pct));
    };

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

void RobotVerifier::addNewBtn() {
    auto const& btnData = g_buttons[rng::get(g_buttons.size() - 1)];

    auto btn = Button::createWithSpriteFrameName(
        btnData.second,
        [this, &id = btnData.first](auto sender) {
            sfx::play(sfx::file::click);

            if (id == m_expected) {
                validateBtns(sender);
            } else {
                m_callback(false);
            };
        });
    btn->setID(btnData.first);

    addChild(btn);

    cue::rescaleToMatch(btn, 27.5f);
};

bool RobotVerifier::init(std::string id, Callback&& cb) {
    m_expected = std::move(id);
    m_callback = std::move(cb);

    if (!CCNode::init()) return false;

    auto layout = RowLayout::create()
                      ->setGap(3.75f)
                      ->setAutoScale(false)
                      ->setGrowCrossAxis(true);

    setID("captcha-verifier");
    setAnchorPoint(anchor::center);
    setContentWidth(125.f);
    setLayout(layout);

    for (int i = 0; i < 16; ++i) {
        addNewBtn();
    };

    updateLayout();

    return true;
};

void RobotVerifier::validateBtns(Button* called) {
    cue::resetNode(called);
    updateLayout();

    for (auto const& btn : getChildrenExt<Button>()) {
        if (btn->getID() == m_expected) {
            addNewBtn();
            updateLayout();

            return;
        };
    };

    m_callback(true);
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