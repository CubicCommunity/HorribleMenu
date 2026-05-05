#include "../MenuCredits.hpp"

#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/ui/GeodeUI.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

bool MenuPlayer::init(ZStringView name, int account, int icon, int color1, int color2, int glowColor) {
    if (!CCNode::init()) return false;

    auto layout = ColumnLayout::create()
                      ->setGap(2.5f)
                      ->setAutoGrowAxis(0.f)
                      ->setAxisAlignment(AxisAlignment::Center)
                      ->setCrossAxisAlignment(AxisAlignment::Start)
                      ->setAutoScale(false);

    setLayout(layout);
    setAnchorPoint({0.5, 0.5});

    auto playerIcon = cue::PlayerIcon::create(IconType::Cube, icon, color1, color2, glowColor);

    addChild(playerIcon);

    auto labelBtn = Button::createWithLabel(
        name,
        "goldFont.fnt",
        [account](auto) {
            if (auto page = ProfilePage::create(account, false)) page->show();
        });
    labelBtn->setID("profile-btn");
    labelBtn->setScale(0.375f);

    playerIcon->setPosition(getScaledContentSize() / 2.f);
    labelBtn->setPosition({getScaledContentWidth() / 2.f, getScaledContentHeight() + (labelBtn->getScaledContentHeight() * 0.875f)});

    addChild(labelBtn, 1);

    updateLayout();

    return true;
};

MenuPlayer* MenuPlayer::create(ZStringView name, int account, int icon, int color1, int color2, int glowColor) {
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
    leadDevLabel->setID("lead-dev-label");
    leadDevLabel->setScale(0.425f);
    leadDevLabel->setAnchorPoint({0.5, 0.5});
    leadDevLabel->setPosition({m_mainLayer->getScaledContentWidth() / 2.f, (m_mainLayer->getScaledContentHeight() / 2.f) + 91.25f});

    m_mainLayer->addChild(leadDevLabel);

    auto leadDevContainerLayout = RowLayout::create()
                                      ->setGap(5.f)
                                      ->setAutoScale(false)
                                      ->setAutoGrowAxis(0.f);

    auto leadDevContainer = CCNode::create();
    leadDevContainer->setID("lead-dev-container");
    leadDevContainer->setAnchorPoint({0.5, 0.5});
    leadDevContainer->setPosition({m_mainLayer->getScaledContentWidth() / 2.f, leadDevLabel->getPositionY() - 35.f});
    leadDevContainer->setLayout(leadDevContainerLayout);

    constexpr LeadDevIcon devs[] = {
        {
            "cheeseworks",
            "Cheeseworks",
            6408873,
            28,
            94,
            98,
            12,
        },
        {
            "arcticwoof",
            "ArcticWoof",
            7689052,
            290,
            83,
            12,
            3,
        },
    };

    m_mainLayer->addChild(leadDevContainer, 1);

    for (auto const& dev : devs) {
        if (auto player = MenuPlayer::create(dev.name, dev.account, dev.icon, dev.color1, dev.color2, dev.glowColor)) {
            player->setID(dev.id);
            leadDevContainer->addChild(player);
        };
    };

    leadDevContainer->updateLayout();

    auto leadDevContainerBg = cue::createBackground(
        {leadDevContainer->getScaledContentWidth() + 10.f, leadDevContainer->getScaledContentHeight() + 6.25f},
        {
            .zOrder = 0,
            .id = "lead-dev-container-bg",
        });
    leadDevContainerBg->setPosition(leadDevContainer->getPosition());

    m_mainLayer->addChild(leadDevContainerBg);

    auto creditsMd = MDTextArea::create(
        "# ![🛠](frame:GJ_hammerIcon_001.png?scale=0.875) Resources\n"
        "**[alk1m123](user:11535118)**: '*[Sapphire SDK](https://www.x.com/GeodeSDK/status/2039225279353176398/)*' logo\n\n"
        "**[Uproxide](user:25397826)**: '*The Yellow One*' sprite from [More Difficulties](mod:uproxide.more_difficulties)\n\n<mod:uproxide.more_difficulties>\n\n"
        "**[Cheeseworks](user:6408873)**: [Mod Developer Branding](mod:cheeseworks.moddevbranding) image for this mod\n\n<mod:cheeseworks.moddevbranding>\n\n"
        "**[dankmeme](user:9735891)**: '*[cue](https://github.com/dankmeme01/cue)*' user interface library\n\n"
        "# ![💝](frame:GJ_diamondsIcon_001.png?scale=0.875) Special Thanks\n"
        "**[Team Avalanche](user:31079132)**: Supporting the project since its experimental days\n\n",
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