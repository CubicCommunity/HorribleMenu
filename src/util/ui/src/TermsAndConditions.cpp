#include "../TermsAndConditions.hpp"

#include <Utils.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

bool TermsAndConditions::init(Callback cb) {
    auto const theme = thisMod->getSettingValue<std::string>("theme");

    if (!Popup::init(420.f, 240.f, themes::getBackgroundSprite(theme))) return false;

    setID("tos"_spr);
    setTitle("Terms and Conditions");
    setKeypadEnabled(false);
    setKeyboardEnabled(false);
    setCloseButtonSpr(CircleButtonSprite::createWithSpriteFrameName("geode.loader/close.png", 0.875f, themes::getCircleBaseColor(theme)));

    m_closeBtn->setVisible(false);
    m_closeBtn->setEnabled(false);

    cursor::show();

    auto tosArea = MDTextArea::create(
        "By using this mod, you agree to the following terms and conditions:\n\n"
        "1. You will not hold the developer liable for any damage caused to your game or account.\n"
        "2. You will not use this mod for cheating or exploiting in multiplayer modes.\n"
        "3. You will not redistribute this mod without permission from the developer.\n"
        "4. You will respect the intellectual property rights of the developer and other modders.\n"
        "5. You will not use this mod for commercial purposes without a license from the developer.\n"
        "6. You will not reverse engineer or decompile this mod without permission from the developer.\n"
        "7. You will not use this mod to create derivative works without permission from the developer.\n"
        "8. You will not use this mod to harass, defame, or discriminate against any individual or group.\n"
        "9. You will not use this mod to distribute malware or harmful software.\n"
        "10. You will not use this mod to violate any applicable laws or regulations.\n\n"
        "By clicking 'Accept', you acknowledge that you have read and agree to these terms and conditions.",
        {380.f, 140.f},
        false);
    tosArea->setPosition({m_mainLayer->getContentSize().width / 2.f, m_mainLayer->getContentSize().height / 2.f + 10.f});

    m_mainLayer->addChild(tosArea);

    auto acceptButton = Button::createWithLabel(
        "Accept",
        "goldFont.fnt",
        [cb](auto) {
            cb(true);
        });

    auto declineButton = Button::createWithLabel(
        "Decline",
        "goldFont.fnt",
        [cb](auto) {
            cb(false);
        });

    m_mainLayer->addChildAtPosition(acceptButton, Anchor::Bottom, {-60.f, 25.f});
    m_mainLayer->addChildAtPosition(declineButton, Anchor::Bottom, {60.f, 25.f});

    return true;
};

TermsAndConditions* TermsAndConditions::create(Callback cb) {
    auto ret = new TermsAndConditions();
    if (ret && ret->init(cb)) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};