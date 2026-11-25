#include "../MenuSafeModePopup.hpp"

#include <Util.h>

#include <rhatt/prelude.h>

#include <ui/Menu.h>
#include <ui/MenuOptionCell.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace cs::brkd::horrible::prelude;

bool MenuSafeModePopup::init() {
    auto theme = mod->getSettingValue<std::string>("theme");
    auto btns = themes::getButtonSquareSprite(theme);

    if (!Popup::init({ 235.0f, 260.0f }, themes::getBackgroundSprite(theme))) return false;

    setID("safe-mode"_spr);
    setTitle("Active Cheats");
    setCloseButtonSpr(themes::createThemeCircleSprite(themes::getCircleBaseColor(theme)));

    auto const options = OptionManager::get()->getEnabledCheats();

    auto bg = cue::createBackground(
        { m_mainLayer->getContentWidth() - 32.5f, m_mainLayer->getContentHeight() - 50.0f },
        {
            .zOrder = 1,
            .id = "",
        }
    );
    m_mainLayer->addChildAtPosition(bg, Anchor::Center, { 0.0f, -7.5f });

    if (options.empty()) {
        auto nothing = Label::create("No cheats enabled", cw::font::big);
        nothing->setAlignment(Label::Alignment::Center);
        nothing->setAnchorPoint({ 0.5f, 0.0f });
        nothing->setLimitLabelWidth(bg->getScaledContentWidth() * 0.875f, 0.475f);
        nothing->setZOrder(9);

        m_mainLayer->addChildAtPosition(nothing, Anchor::Center, { 0.0f, -6.75f });

        auto nothingDesc = Label::createRich("<cr>Safe Mode</c> might be enabled <cy>manually</c>, or <co>another mod enabled anti-cheat</c>.", cw::font::chat);
        nothingDesc->setScale(0.625f);
        nothingDesc->setAnchorPoint({ 0.5f, 1.0f });
        nothingDesc->setMaxWidth((bg->getScaledContentWidth() * 0.95f) / nothingDesc->getScale());
        nothingDesc->setBreakWords(true);
        nothingDesc->setAlignment(Label::Alignment::Center);
        nothingDesc->setZOrder(9);

        m_mainLayer->addChildAtPosition(nothingDesc, Anchor::Center, { 0.0f, -8.25f });

    } else {
        auto list = ScrollLayer::create(bg->getScaledContentSize() * 0.925f);
        list->setID("options-list");
        list->setZOrder(9);

        list->m_contentLayer->setLayout(ScrollLayer::createDefaultListLayout());

        m_mainLayer->addChildAtPosition(list, Anchor::Center, { 0.0f, -7.5f });

        for (auto const& opt : options) list->m_contentLayer->addChild(MenuOptionCell::createView(list->getScaledContentWidth(), opt, theme));

        list->m_contentLayer->updateLayout();
        list->scrollToTop();
    };

    auto settingsBtn = Button::createWithNode(
        CircleButtonSprite::createWithSpriteFrameName(
            "settings.png"_gode,
            1.0f,
            themes::getCircleBaseColor(theme)
        ),
        [](Button* sender) {
            openSettingsPopup(mod);
        }
    );
    settingsBtn->setID("settings-btn");
    settingsBtn->setScale(0.625f);

    m_mainLayer->addChildAtPosition(settingsBtn, Anchor::BottomRight);

    auto btn = Button::createWithNode(
        ButtonSprite::create(
            "Manage",
            cw::font::gold,
            btns,
            0.875f
        ),
        [this, t = std::move(theme)](Button* sender) mutable {
            Menu::create(std::move(t))->show();
            this->onClose(sender);
        }
    );
    btn->setID("manage-options-btn");
    btn->setScale(0.825f);

    m_mainLayer->addChildAtPosition(btn, Anchor::Bottom);

    cw::addHelpButton(
        m_mainLayer,
        "In this pop-up, you can see any <cg>cheat options you have enabled</c>, which may be <cr>triggering Auto Safe Mode</c>."
    );

    return true;
}

MenuSafeModePopup* MenuSafeModePopup::create() {
    auto ret = new MenuSafeModePopup();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
}