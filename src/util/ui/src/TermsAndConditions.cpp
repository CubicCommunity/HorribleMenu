#include "../TermsAndConditions.hpp"

#include <Utils.h>

#include <Geode/Geode.hpp>

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

    auto acceptBtn = Button::createWithNode(
        ButtonSprite::create(
            "Accept",
            "bigFont.fnt",
            themes::getButtonSquareSprite(theme)),
        [this, cb](auto) {
            sfx::play(sfx::file::good);
            if (cb) cb(true);
            removeFromParent();
        });
    acceptBtn->setScale(0.75f);

    auto declineBtn = Button::createWithNode(
        ButtonSprite::create(
            "Decline",
            "goldFont.fnt",
            themes::getButtonSquareSprite(theme)),
        [this, cb](auto) {
            sfx::play(sfx::file::bad);
            if (cb) cb(false);
            removeFromParent();
        });
    declineBtn->setScale(0.75f);

    m_mainLayer->addChildAtPosition(acceptBtn, Anchor::Bottom, {-60.f, 25.f});
    m_mainLayer->addChildAtPosition(declineBtn, Anchor::Bottom, {60.f, 25.f});

    if (auto acceptBtnSpr = typeinfo_cast<ButtonSprite*>(acceptBtn->getDisplayNode())) {
        acceptBtn->setEnabled(false);
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

TermsAndConditions* TermsAndConditions::create(Callback&& cb) {
    auto ret = new TermsAndConditions();
    if (ret->init(std::move(cb))) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};