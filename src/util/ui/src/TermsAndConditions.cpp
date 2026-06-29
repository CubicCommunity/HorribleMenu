#include "../TermsAndConditions.hpp"

#include <numbers>
#include <Utils.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

struct TermsAndConditions::Impl final {
    Button* acceptBtn = nullptr;

    CCPoint acceptVelocity = {0.f, 0.f};

    Callback callback;
};

TermsAndConditions::TermsAndConditions() : m_impl(std::make_unique<Impl>()) {};
TermsAndConditions::~TermsAndConditions() {};

bool TermsAndConditions::init(Callback&& cb) {
    m_impl->callback = std::move(cb);

    auto const theme = mod->getSettingValue<std::string>("theme");

    if (!Popup::init(420.f, 240.f, themes::getBackgroundSprite(theme))) return false;

    setID("tos"_spr);
    setTitle("Terms and Conditions");
    setKeypadEnabled(false);
    setKeyboardEnabled(false);
    setCloseButtonSpr(CircleButtonSprite::createWithSpriteFrameName(themes::close, 0.875f, themes::getCircleBaseColor(theme)));

    popup::closeBtnID(m_closeBtn);

    m_closeBtn->setVisible(false);
    m_closeBtn->setEnabled(false);

    addSideArt(m_mainLayer, SideArt::All, SideArtStyle::PopupBlue);

    cursor::show();

    auto tosArea = MDTextArea::create(
        "By using this mod, you agree to the following terms and conditions:\n\n"
        "1. You will NOT make fun of the guy who's second in the *Thanks* list.\n"
        "2. You will love and adore Breakeode and buy all of their merch.\n"
        "3. You will consent to using this mod forever and ever.\n"
        "4. You will used Charged Leap to assert your dominance over other Globed players.\n"
        "5. You will not hate Level Ads.\n"
        "6. You will NOT tell us Horrible Menu isn't perfect.\n"
        "7. You will NOT!!!\n"
        "8. You will love it when we release the next update. Just one update.\n"
        "9. You will hate it when we release the next update. Just one update.\n"
        "10. You will not use this mod to violate any applicable laws or regulations.\n\n"
        "**By clicking 'Accept', you acknowledge that you have read and agree to these terms and conditions.**\n\n"
        "---\n\n"
        "For legal reasons, this is all a joke..!",
        {385.f, 145.f},
        false);
    tosArea->setPosition({m_mainLayer->getContentSize().width / 2.f, (m_mainLayer->getContentSize().height / 2.f) + 5.f});

    m_mainLayer->addChild(tosArea);

    m_impl->acceptBtn = Button::createWithNode(
        ButtonSprite::create(
            "Accept",
            font::big,
            themes::getButtonSquareSprite(theme)),
        [this](auto) {
            sfx::play(sfx::file::good);

            if (m_impl->callback) m_impl->callback(true);
            if (auto pl = PlayLayer::get()) pl->resume();

            removeFromParent();
        });
    m_impl->acceptBtn->setZOrder(10);
    m_impl->acceptBtn->setScale(0.75f);

    auto declineBtn = Button::createWithNode(
        ButtonSprite::create(
            "Decline",
            font::big,
            themes::getButtonSquareSprite(theme)),
        [this](auto) {
            sfx::play(sfx::file::bad);

            if (m_impl->callback) m_impl->callback(false);

            removeFromParent();
        });
    declineBtn->setZOrder(10);
    declineBtn->setScale(0.75f);

    m_mainLayer->addChildAtPosition(m_impl->acceptBtn, Anchor::Bottom, {-60.f, 25.f});
    m_mainLayer->addChildAtPosition(declineBtn, Anchor::Bottom, {60.f, 25.f});

    auto const angle = rng::get(2.f * std::numbers::pi);
    m_impl->acceptVelocity = ccp(cosf(angle), sinf(angle));
    scheduleUpdate();

    if (auto acceptBtnSpr = typeinfo_cast<ButtonSprite*>(m_impl->acceptBtn->getDisplayNode())) {
        m_impl->acceptBtn->setEnabled(false);
        acceptBtnSpr->setOpacity(0);

        acceptBtnSpr->runAction(CCSpawn::createWithTwoActions(
            CCEaseBounceIn::create(
                CCFadeTo::create(rng::get(10.f, 1.25f), 255)),
            CCCallFuncN::create(this, callfuncN_selector(TermsAndConditions::finishBtnFade))));
    };

    sfx::play(sfx::file::pop);

    return true;
};

void TermsAndConditions::finishBtnFade(CCNode* sender) {
    if (auto btn = typeinfo_cast<Button*>(sender->getParent())) btn->setEnabled(true);
};

void TermsAndConditions::update(float dt) {
    if (!m_impl->acceptBtn) return;

    CCSize const winSize = CCDirector::sharedDirector()->getWinSize();

    CCPoint const localPos = m_impl->acceptBtn->getPosition();
    CCPoint worldPos = m_mainLayer->convertToWorldSpace(localPos);

    CCSize const buttonSize = m_impl->acceptBtn->getScaledContentSize();

    CCPoint const mousePos = cocos::getMousePos();
    CCPoint const away = ccpSub(worldPos, mousePos);

    float const dist = std::sqrt(away.x * away.x + away.y * away.y);
    float speed = 100.f;  // default speed

    // make the mouse (like cheeseworks) to chase the accept button cuz im evil
    if (dist < 25.f && dist > 0.f) {
        auto normalizedAway = ccp(away.x / dist, away.y / dist);

        auto currentVelLen = std::sqrt(m_impl->acceptVelocity.x * m_impl->acceptVelocity.x + m_impl->acceptVelocity.y * m_impl->acceptVelocity.y);
        if (currentVelLen > 0.f) {
            normalizedAway.x += m_impl->acceptVelocity.x / currentVelLen;
            normalizedAway.y += m_impl->acceptVelocity.y / currentVelLen;
        };

        auto const newLen = std::sqrt(normalizedAway.x * normalizedAway.x + normalizedAway.y * normalizedAway.y);
        if (newLen > 0.f) {
            normalizedAway.x /= newLen;
            normalizedAway.y /= newLen;
            m_impl->acceptVelocity = normalizedAway;
        };

        // mouse avoid multiplier
        speed *= 1.75f;
    };

    worldPos = ccpAdd(worldPos, ccpMult(m_impl->acceptVelocity, speed * dt));

    auto const halfW = buttonSize.width / 2.f;
    auto const halfH = buttonSize.height / 2.f;

    if (worldPos.x < halfW) {
        worldPos.x = halfW;
        m_impl->acceptVelocity.x = fabsf(m_impl->acceptVelocity.x);
    } else if (worldPos.x > winSize.width - halfW) {
        worldPos.x = winSize.width - halfW;
        m_impl->acceptVelocity.x = -fabsf(m_impl->acceptVelocity.x);
    };

    if (worldPos.y < halfH) {
        worldPos.y = halfH;
        m_impl->acceptVelocity.y = fabsf(m_impl->acceptVelocity.y);
    } else if (worldPos.y > winSize.height - halfH) {
        worldPos.y = winSize.height - halfH;
        m_impl->acceptVelocity.y = -fabsf(m_impl->acceptVelocity.y);
    };

    m_impl->acceptBtn->setPosition(m_mainLayer->convertToNodeSpace(worldPos));
};

TermsAndConditions* TermsAndConditions::create(Callback&& cb) {
    auto ret = new TermsAndConditions();
    if (ret->init(std::move(cb))) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};