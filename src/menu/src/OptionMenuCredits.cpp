#include "../OptionMenuCredits.h"

#include <Utils.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

OptionMenuCredits* OptionMenuCredits::s_inst = nullptr;

bool OptionMenuCredits::init(ZStringView theme) {
    auto btns = themes::getCircleBaseColor(theme);

    if (!Popup::init(375.f, 250.f, themes::getBackgroundSprite(theme))) return false;

    setID("credits"_spr);
    setTitle("Credits");
    setCloseButtonSpr(CircleButtonSprite::createWithSpriteFrameName("geode.loader/close.png", 0.875f, btns, CircleBaseSize::Small));

    addSideArt(m_mainLayer, SideArt::All, SideArtStyle::PopupGold);

    return true;
};

void OptionMenuCredits::onClose(CCObject* sender) {
    s_inst = nullptr;
    Popup::onClose(sender);
};

void OptionMenuCredits::onExit() {
    s_inst = nullptr;
    Popup::onExit();
};

void OptionMenuCredits::cleanup() {
    s_inst = nullptr;
    Popup::cleanup();
};

OptionMenuCredits* OptionMenuCredits::get() noexcept {
    return s_inst;
};

OptionMenuCredits* OptionMenuCredits::create(ZStringView theme) {
    auto ret = new OptionMenuCredits();
    if (ret->init(theme)) {
        ret->autorelease();
        s_inst = ret;
        return ret;
    };

    delete ret;
    return nullptr;
};