#include "../MenuCredits.h"

#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/ui/GeodeUI.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

bool MenuPlayer::init(ZStringView name, int account, int icon, ccColor3B const& color1, ccColor3B const& color2, ccColor3B const& glowColor) {
    if (!CCNode::init()) return false;

    setAnchorPoint({0.5, 0.5});
    setContentSize({30.f, 30.f});

    auto playerIcon = SimplePlayer::create(icon);
    playerIcon->setColor(color1);
    playerIcon->setSecondColor(color2);
    playerIcon->setGlowOutline(glowColor);

    playerIcon->setPosition(getScaledContentSize() / 2.f);

    addChild(playerIcon);

    auto label = CCLabelBMFont::create(name.c_str(), "goldFont.fnt");
    label->setScale(0.375f);

    if (label->getScaledContentWidth() > getScaledContentWidth()) setContentWidth(label->getScaledContentWidth());

    auto labelBtn = Button::createWithNode(
        label,
        [account](auto) {
            if (auto page = ProfilePage::create(account, false)) page->show();
        });
    labelBtn->setID("profile-btn");
    labelBtn->setPosition({getScaledContentWidth() / 2.f, getScaledContentHeight() + (label->getScaledContentHeight() * 0.875f)});

    playerIcon->setPositionX(getScaledContentWidth() / 2.f);

    addChild(labelBtn, 1);

    return true;
};

MenuPlayer* MenuPlayer::create(ZStringView name, int account, int icon, ccColor3B const& color1, ccColor3B const& color2, ccColor3B const& glowColor) {
    auto ret = new MenuPlayer();
    if (ret->init(name, account, icon, color1, color2, glowColor)) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};

MenuCredits* MenuCredits::s_inst = nullptr;

bool MenuCredits::init(ZStringView theme) {
    auto btns = themes::getCircleBaseColor(theme);

    if (!Popup::init(400.f, 265.f, themes::getBackgroundSprite(theme))) return false;

    setID("credits"_spr);
    setTitle("Credits");
    setCloseButtonSpr(CircleButtonSprite::createWithSpriteFrameName(themes::close, 0.875f, btns, CircleBaseSize::Small));

    addSideArt(m_mainLayer, SideArt::All, SideArtStyle::PopupGold);

    auto leadDevLabel = CCLabelBMFont::create("Lead Developers", "bigFont.fnt");
    leadDevLabel->setID("lead-devs-label");
    leadDevLabel->setScale(0.375f);
    leadDevLabel->setAnchorPoint({0.5, 0.5});
    leadDevLabel->setPosition({m_mainLayer->getScaledContentWidth() / 2.f, (m_mainLayer->getScaledContentHeight() / 2.f) + 90.f});

    m_mainLayer->addChild(leadDevLabel);

    auto leadDevContainerLayout = RowLayout::create()
                                      ->setGap(5.f)
                                      ->setAutoScale(false)
                                      ->setAutoGrowAxis(0.f);

    auto leadDevContainer = CCNode::create();
    leadDevContainer->setID("lead-devs-container");
    leadDevContainer->setAnchorPoint({0.5, 0.5});
    leadDevContainer->setPosition({m_mainLayer->getScaledContentWidth() / 2.f, leadDevLabel->getPositionY() - 40.f});
    leadDevContainer->setLayout(leadDevContainerLayout);

    constexpr LeadDevIcon devs[] = {
        {
            "cheeseworks",
            "Cheeseworks",
            6408873,
            28,
            {64, 64, 64},
            {252, 181, 255},
            {255, 255, 255},
        },
        {
            "arcticwoof",
            "ArcticWoof",
            7689052,
            290,
            {160, 255, 255},
            {255, 255, 255},
            {0, 255, 255},
        },
    };

    for (auto const& dev : devs) {
        if (auto player = MenuPlayer::create(dev.name, dev.account, dev.icon, dev.color1, dev.color2, dev.glowColor)) {
            player->setID(dev.id);
            leadDevContainer->addChild(player);
        };
    };

    m_mainLayer->addChild(leadDevContainer);
    leadDevContainer->updateLayout();

    constexpr auto creditsMdTxt =
        "# ![🛠](frame:GJ_hammerIcon_001.png?scale=0.875) Resources\n"
        "**[alk1m123](user:11535118)**: '*[Sapphire SDK](https://www.x.com/GeodeSDK/status/2039225279353176398/)*' logo\n\n"
        "**[Uproxide](user:25397826)**: '*The Yellow One*' sprite from [More Difficulties](mod:uproxide.more_difficulties)\n\n<mod:uproxide.more_difficulties>\n\n"
        "**[Cheeseworks](user:6408873)**: [Mod Developer Branding](mod:cheeseworks.moddevbranding) image for this mod\n\n<mod:cheeseworks.moddevbranding>\n\n"
        "# ![💝](frame:GJ_diamondsIcon_001.png?scale=0.875) Special Thanks\n"
        "**[Team Avalanche](user:31079132)**: Supporting the project since its experimental days\n\n"
        "**[dankmeme](user:9735891)**: helped with pretty important programming decisions :D\n\n";

    auto creditsMd = MDTextArea::create(
        creditsMdTxt,
        {m_mainLayer->getScaledContentWidth() - 55.f,
            140.f});
    creditsMd->setID("credits");
    creditsMd->setPosition({m_mainLayer->getScaledContentWidth() / 2.f, 90.f});

    m_mainLayer->addChild(creditsMd, 1);

    auto websiteBtn = Button::createWithNode(
        ButtonSprite::create(
            "Website",
            "bigFont.fnt",
            themes::getButtonSquareSprite(theme)),
        [](auto) {
            createQuickPopup(
                "Breakeode",
                "Visit <cr>Breakeode</c>'s official website?",
                "Cancel",
                "OK",
                [](auto, bool ok) {
                    if (ok) web::openLinkInBrowser("https://breakeode.cubicstudios.xyz/");
                });
        });
    websiteBtn->setID("website-btn");
    websiteBtn->setScale(0.625f);
    websiteBtn->setPosition({m_mainLayer->getScaledContentWidth() / 2.f, 0.f});

    m_mainLayer->addChild(websiteBtn, 1);

    auto modBtn = Button::createWithNode(
        CircleButtonSprite::createWithSpriteFrameName(
            "geode.loader/geode-logo-outline-gold.png",
            1.f,
            btns),
        [](auto) {
            openInfoPopup(thisMod);
        });
    modBtn->setID("mod-information-btn");
    modBtn->setScale(0.5f);
    modBtn->setPosition({15.f, 15.f});

    m_mainLayer->addChild(modBtn, 9);

    if (auto om = OptionManager::get()) {
        auto integrations = om->getMods();
        auto str = (integrations.size() > 0)
                       ? fmt::format("Here's the <cg>full list of your installed mods (**`{}`**)</c> which <cy>currently add their own features as options to [Horrible Menu](mod:cubicstudios.horriblemenu)</c>.\n\n---", integrations.size())
                       : "If <cg>different mods</c> use this mod's API to <cy>add their own options</c>, they will all be listed here.";

        if (integrations.size() > 0) {
            for (auto const& i : integrations) {
                log::trace("Listing mod {}", i->getID());
                str = fmt::format("{}\n\n<mod:{}>", str, i->getID());
            };
        };

        auto integrationsBtn = Button::createWithNode(
            CircleButtonSprite::createWithSpriteFrameName(
                "geode.loader/grid-view.png",
                0.75f,
                btns),
            [integrations = std::move(integrations), str = std::move(str)](auto) {
                if (integrations.size() > 0) {
                    MDPopup::create(
                        "Integrations",
                        str,
                        "OK")
                        ->show();
                } else {
                    createQuickPopup(
                        "Integrations",
                        str,
                        "OK",
                        nullptr,
                        nullptr);
                };
            });
        integrationsBtn->setID("integrations-btn");
        integrationsBtn->setScale(0.5f);
        integrationsBtn->setPosition({m_mainLayer->getScaledContentWidth() - 15.f, 15.f});

        m_mainLayer->addChild(integrationsBtn, 9);
    };

    auto infoBtn = Button::createWithSpriteFrameName(
        "GJ_infoIcon_001.png",
        [this](auto) {
            createQuickPopup(
                "Help",
                "This menu aims to give credit to everyone who has <cy>contributed to the development of Horrible Menu</c>, directly or indirectly.\n\n"
                "<co>If we missed anyone, let us know by opening an issue about it on our GitHub repository!</c>\n\n"
                "<cd>Thanks to everyone who has helped this project in any way! We greatly appreciate you! <3</c>",
                "OK",
                nullptr,
                365.f,
                nullptr);
        });
    infoBtn->setID("info-btn");
    infoBtn->setScale(0.75f);
    infoBtn->setPosition(m_mainLayer->getScaledContentSize() - 13.75f);

    m_mainLayer->addChild(infoBtn, 9);

    return true;
};

void MenuCredits::onExit() {
    s_inst = nullptr;
    Popup::onExit();
};

MenuCredits* MenuCredits::get() noexcept {
    return s_inst;
};

MenuCredits* MenuCredits::create(ZStringView theme) {
    auto ret = new MenuCredits();
    if (ret->init(theme)) {
        ret->autorelease();
        s_inst = ret;
        return ret;
    };

    delete ret;
    return nullptr;
};