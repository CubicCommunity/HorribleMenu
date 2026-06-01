#include "../TermsAndConditions.hpp"

#include <numbers>
#include <Utils.h>

#include <Geode/Geode.hpp>
#include "Geode/cocos/cocoa/CCGeometry.h"

using namespace geode::prelude;
using namespace horrible::prelude;

bool TermsAndConditions::init(Callback&& cb) {
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
        "4. You will respect the intellectual... Yeah, that's it.\n"
        "5. You will not hate Level Ads.\n"
        "6. You will NOT tell us Horrible Menu isn't perfect.\n"
        "7. You will NOT!!!\n"
        "8. You will love it when we release the next update. Just one update.\n"
        "9. You will hate it when we release the next update. Just one update.\n"
        "10. You will not use this mod to violate any applicable laws or regulations.\n\n"
        "**By clicking 'Accept', you acknowledge that you have read and agree to these terms and conditions.**\n\n"
        "---\n\n"
        "For legal reasons, this is all a joke..!",
        {380.f, 145.f},
        false);
    tosArea->setPosition({m_mainLayer->getContentSize().width / 2.f, (m_mainLayer->getContentSize().height / 2.f) + 5.f});

    m_mainLayer->addChild(tosArea);

    m_acceptBtn = Button::createWithNode(
        ButtonSprite::create(
            "Accept",
            font::big,
            themes::getButtonSquareSprite(theme)),
        [this, cb](auto) {
            sfx::play(sfx::file::good);
            if (cb) cb(true);
            removeFromParent();
        });
    m_acceptBtn->setScale(0.75f);

    auto declineBtn = Button::createWithNode(
        ButtonSprite::create(
            "Decline",
            font::big,
            themes::getButtonSquareSprite(theme)),
        [this, cb](auto) {
            sfx::play(sfx::file::bad);
            if (cb) cb(false);
            removeFromParent();
        });
    declineBtn->setScale(0.75f);

    m_mainLayer->addChildAtPosition(m_acceptBtn, Anchor::Bottom, {-60.f, 25.f});
    m_mainLayer->addChildAtPosition(declineBtn, Anchor::Bottom, {60.f, 25.f});

    auto const angle = rng::get(2.f * std::numbers::pi);
    m_acceptVelocity = ccp(cosf(angle), sinf(angle));
    scheduleUpdate();

    if (auto acceptBtnSpr = typeinfo_cast<ButtonSprite*>(m_acceptBtn->getDisplayNode())) {
        m_acceptBtn->setEnabled(false);
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
    if (!m_acceptBtn) return;

    CCSize const winSize = CCDirector::sharedDirector()->getWinSize();
    CCPoint pos = m_acceptBtn->getPosition();
    CCSize const buttonSize = m_acceptBtn->getScaledContentSize();

    CCPoint const mousePos = cocos::getMousePos();
    CCPoint const away = ccpSub(pos, mousePos);
    float const dist = std::sqrt(away.x * away.x + away.y * away.y);
    float speed = m_acceptSpeed;

    if (dist < m_mouseAvoidDistance && dist > 0.f) {
        auto normalizedAway = ccp(away.x / dist, away.y / dist);
        auto currentVelLen = std::sqrt(m_acceptVelocity.x * m_acceptVelocity.x + m_acceptVelocity.y * m_acceptVelocity.y);
        if (currentVelLen > 0.f) {
            normalizedAway.x += m_acceptVelocity.x / currentVelLen;
            normalizedAway.y += m_acceptVelocity.y / currentVelLen;
        }
        auto const newLen = std::sqrt(normalizedAway.x * normalizedAway.x + normalizedAway.y * normalizedAway.y);
        if (newLen > 0.f) {
            normalizedAway.x /= newLen;
            normalizedAway.y /= newLen;
            m_acceptVelocity = normalizedAway;
        }
        speed *= m_mouseAvoidMultiplier;
    }

    auto const delta = ccpMult(m_acceptVelocity, speed * dt);
    pos = ccpAdd(pos, delta);

    auto const halfW = buttonSize.width / 2.f;
    auto const halfH = buttonSize.height / 2.f;

    if (pos.x < halfW) {
        pos.x = halfW;
        m_acceptVelocity.x = fabsf(m_acceptVelocity.x);
    } else if (pos.x > winSize.width - halfW) {
        pos.x = winSize.width - halfW;
        m_acceptVelocity.x = -fabsf(m_acceptVelocity.x);
    }

    if (pos.y < halfH) {
        pos.y = halfH;
        m_acceptVelocity.y = fabsf(m_acceptVelocity.y);
    } else if (pos.y > winSize.height - halfH) {
        pos.y = winSize.height - halfH;
        m_acceptVelocity.y = -fabsf(m_acceptVelocity.y);
    }

    m_acceptBtn->setPosition(pos);
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