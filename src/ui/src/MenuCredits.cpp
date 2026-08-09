#include "../MenuCredits.hpp"

#include <Util.h>

#include <Geode/Geode.hpp>

#include <Geode/ui/GeodeUI.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

// hardcoded lead devs for this build of horrible menu if web request fails
static auto const g_defaultDevs = std::to_array<LeadDevIcon>({
    {
        .id = "cheeseworks",
        .name = "Cheeseworks",
        .account = 6408873,
        .icon = 28,
        .color1 = 94,
        .color2 = 98,
        .glowColor = 12,
    },
});

$on_mod(Loaded) {
    if (auto cm = CreditsManager::get()) cm->loadLeadDevs();
};

Result<LeadDevIcon> matjson::Serialize<LeadDevIcon>::fromJson(matjson::Value const& value) {
    if (!value.isObject()) return Err("Expected an object");

    GEODE_UNWRAP_INTO(std::string id, value["id"].asString());
    GEODE_UNWRAP_INTO(std::string name, value["name"].asString());
    GEODE_UNWRAP_INTO(int account, value["account"].asInt());
    GEODE_UNWRAP_INTO(int icon, value["icon"].asInt());
    GEODE_UNWRAP_INTO(int color1, value["color1"].asInt());
    GEODE_UNWRAP_INTO(int color2, value["color2"].asInt());
    GEODE_UNWRAP_INTO(int glowColor, value["glowColor"].asInt());

    return Ok(LeadDevIcon{std::move(id), std::move(name), account, icon, color1, color2, glowColor});
};

matjson::Value matjson::Serialize<LeadDevIcon>::toJson(LeadDevIcon const& value) {
    auto obj = matjson::Value();

    obj["id"] = value.id;
    obj["name"] = value.name;
    obj["account"] = value.account;
    obj["icon"] = value.icon;
    obj["color1"] = value.color1;
    obj["color2"] = value.color2;
    obj["glowColor"] = value.glowColor;

    return obj;
};

bool MenuPlayer::init(ZStringView name, int account, int icon, int color1, int color2, int glowColor) {
    if (!CCNode::init()) return false;

    auto layout = ColumnLayout::create()
                      ->setGap(2.5f)
                      ->setAutoGrowAxis(0.f)
                      ->setAxisAlignment(AxisAlignment::Center)
                      ->setCrossAxisAlignment(AxisAlignment::Start)
                      ->setAutoScale(false);

    setLayout(layout);
    setAnchorPoint(anchor::center);

    auto playerIcon = cue::PlayerIcon::create(IconType::Cube, icon, color1, color2, glowColor);

    addChild(playerIcon);

    auto labelBtn = Button::createWithLabel(
        name,
        font::gold,
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

    popup::closeBtnID(m_closeBtn);

    addSideArt(m_mainLayer, SideArt::All, SideArtStyle::PopupGold);

    auto leadDevLabel = CCLabelBMFont::create("Lead Developers", font::big);
    leadDevLabel->setID("lead-dev-label");
    leadDevLabel->setScale(0.425f);
    leadDevLabel->setAnchorPoint({0, 0.5});
    leadDevLabel->setPosition({30.f, (m_mainLayer->getScaledContentHeight() / 2.f) + 91.25f});

    m_mainLayer->addChild(leadDevLabel);

    auto leadDevContainerLayout = RowLayout::create()
                                      ->setGap(5.f)
                                      ->setAutoScale(false)
                                      ->setAutoGrowAxis(0.f);

    auto leadDevContainer = CCNode::create();
    leadDevContainer->setID("lead-dev-container");
    leadDevContainer->setAnchorPoint({anchor::center});
    leadDevContainer->setLayout(leadDevContainerLayout);

    m_mainLayer->addChild(leadDevContainer, 1);

    for (auto const& dev : CreditsManager::get()->getLeadDevs()) {
        if (auto player = MenuPlayer::create(dev.name, dev.account, dev.icon, dev.color1, dev.color2, dev.glowColor)) {
            player->setID(dev.id);
            leadDevContainer->addChild(player);
        };
    };

    leadDevContainer->updateLayout();

    leadDevContainer->setPosition({2.5f + leadDevLabel->getPositionX() + (leadDevContainer->getScaledContentWidth() / 2.f), leadDevLabel->getPositionY() - 35.f});

    auto leadDevContainerBg = cue::createBackground(
        {leadDevContainer->getScaledContentWidth() + 10.f, leadDevContainer->getScaledContentHeight() + 6.25f},
        {
            .zOrder = 0,
            .id = "",
        });
    leadDevContainerBg->setPosition(leadDevContainer->getPosition());

    m_mainLayer->addChild(leadDevContainerBg);

    auto resrcBtnContainerLayout = RowLayout::create()
                                       ->setGap(2.5f)
                                       ->setAutoScale(false)
                                       ->setGrowCrossAxis(true);

    auto resrcBtnContainer = CCNode::create();
    resrcBtnContainer->setID("resources-container");
    resrcBtnContainer->setAnchorPoint({0, 0.5});
    resrcBtnContainer->setContentSize({m_mainLayer->getScaledContentWidth() - 198.75f, leadDevContainer->getScaledContentHeight()});
    resrcBtnContainer->setPosition({leadDevContainer->getPositionX() + (leadDevContainerBg->getScaledContentWidth() / 2.f) + 8.75f, leadDevContainer->getPositionY()});
    resrcBtnContainer->setLayout(resrcBtnContainerLayout);

    m_mainLayer->addChild(resrcBtnContainer, 9);

    auto resrcBtns = std::to_array<ResourceButton>({
        {
            "breakeode-support-btn",
            "Need Help?",
            [](auto) {
                createQuickPopup(
                    "Breakeode Support",
                    "If <cg>Horrible Menu</c> <cy>is causing issues</c>, you may reach out to the <cl>Breakeode</c> development team by <cy>creating a support ticket in the Discord server</c>. If you have any other questions, feel free to ask!\n\n"
                    "Would you like to join <cl>Breakeode</c>'s <cj>support Discord server</c>?",
                    "Cancel",
                    "OK",
                    [](auto, bool ok) {
                        if (ok) web::openLinkInBrowser("https://dsc.gg/breakeode");
                    });
            },
        },
        {
            "changelog-btn",
            "What's New?",
            [](auto) {
                openChangelogPopup(mod);
            },
        },
    });

    for (auto& b : resrcBtns) {
        auto btn = Button::createWithNode(
            ButtonSprite::create(
                b.label,
                font::gold,
                themes::getButtonSquareSprite(theme)),
            std::move(b.callback));
        btn->setID(b.id);
        btn->setScale(0.75f);

        resrcBtnContainer->addChild(btn);
    };

    resrcBtnContainer->updateLayout();

    auto creditsMd = MDTextArea::create(
        "# ![🛠](frame:GJ_hammerIcon_001.png?scale=0.875) Resources\n"
        "**[Geode Team](mod:geode.loader)**: '*[Geode SDK](https://github.com/geode-sdk/geode)*' Geometry Dash modding framework\n\n"
        "**[Presta](user:540196)**: '*[Congregation](level:68668045)*' jumpscare level\n\n"
        "**[IcEDCave](user:1504161)**: '*Grief*' jumpscare level\n\n"
        "**[sink](user:5871590)**: '*[Tidal Wave](level:93733469)*' jumpscare level\n\n"
        "**[jackaezie](user:9456524)**: '*[YOUVE BEEN TROLLED](level:57436521)*' jumpscare level\n\n"
        "**[alk1m123](user:11535118)**: '*[Sapphire SDK](https://www.x.com/GeodeSDK/status/2039225279353176398/)*' logo\n\n"
        "**[Uproxide](user:25397826)**: '*The Yellow One*' sprite from [More Difficulties](mod:uproxide.more_difficulties)\n\n<mod:uproxide.more_difficulties>\n\n"
        "**[Cheeseworks](user:6408873)**: [Mod Developer Branding](mod:cheeseworks.moddevbranding) image for this mod\n\n<mod:cheeseworks.moddevbranding>\n\n"
        "**[dankmeme](user:9735891)**: '*[cue](https://github.com/dankmeme01/cue)*' user interface library, '*[asp2](https://github.com/dankmeme01/asp2)*' utility library, '*[arc](https://github.com/dankmeme01/arc)*' async runtime library\n\n"
        "# ![💝](frame:GJ_diamondsIcon_001.png?scale=0.875) Special Thanks\n"
        "**[Cheeseworks](user:6408873)**: Internals, API/DX, UI/UX, options' features, mod branding\n\n"
        "**[ArcticWoof](user:7689052)**: UI, options' features, Horrible Menu logo, original idea for this mod\n\n"
        "**[Team Avalanche](user:31079132)**: Supporting the project since its experimental days\n\n"
        "# ![📖](frame:accountBtn_myLists_001.png?scale=0.5) License & Attribution\n"
        "The current build of [Horrible Menu](mod:cubicstudios.horriblemenu) is licensed under the [GNU General Public License v3.0 (GPL-3.0)](https://github.com/CubicCommunity/HorribleMenu/blob/main/LICENSE.md).\n\n"
        "Third-party assets, API modifications, libraries, and other external resources are credited in their respective sections above.",
        {
            m_mainLayer->getScaledContentWidth() - 55.f,
            140.f,
        });
    creditsMd->setID("credits");
    creditsMd->setPosition({m_mainLayer->getScaledContentWidth() / 2.f, 90.f});

    m_mainLayer->addChild(creditsMd, 1);

    auto websiteBtn = Button::createWithNode(
        ButtonSprite::create(
            "Website",
            font::big,
            themes::getButtonSquareSprite(theme)),
        [](auto) {
            createQuickPopup(
                "Breakeode",
                "Visit <cl>Breakeode</c>'s official website?",
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

    // 1% chance to show sapphire logo over geode
    auto isModBtnSecret = rng::fast() <= 1;

    auto modBtn = Button::createWithNode(
        CircleButtonSprite::createWithSpriteFrameName(
            isModBtnSecret ? themes::getIconSprite(themes::icons::SapphireSDK) : "geode.loader/geode-logo-outline-gold.png",
            isModBtnSecret ? 1.125f : 1.f,
            btns),
        [](auto) {
            openInfoPopup(mod);
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

void CreditsManager::loadLeadDevs() {
    log::debug("Requested to fetch data on lead developers");

#define LEAD_DEVS_INTERNAL(container, dev)                        \
    log::trace("Pushing {} to lead developer list...", dev.name); \
    container.push_back(dev)

#define LEAD_DEVS(container, array)         \
    for (auto const& dev : array) {         \
        LEAD_DEVS_INTERNAL(container, dev); \
    }

    if (m_leadDevs.size() <= 0) {
        log::info("Sending web request for lead developer credits");

        async::spawn(
            web::WebRequest().get("https://api.cubicstudios.xyz/breakeode/v1/horrible/credits"),
            [this](web::WebResponse res) {
                static auto const fallback = [this](std::string_view err = "") {
                    log::error("Lead Developer credits web request failed ({}), falling back to defaults", err);
                    LEAD_DEVS(m_leadDevs, g_defaultDevs);
                };

                if (res.error()) return fallback(res.errorMessage());

                auto jsonRes = res.json();
                if (jsonRes.isErr()) return fallback(std::move(jsonRes).unwrapErr());

                auto json = std::move(jsonRes).unwrap();

                auto arrayRes = json.asArray();
                if (arrayRes.isErr()) return fallback(std::move(arrayRes).unwrapErr());

                auto array = std::move(arrayRes).unwrap();

                for (auto const& val : array) {
                    LEAD_DEVS_INTERNAL(m_leadDevs, matjson::Serialize<LeadDevIcon>::fromJson(val).unwrapOrDefault());
                };
            });
    } else {
        log::error("Lead developer data already populated");
    };
};

std::span<const LeadDevIcon> CreditsManager::getLeadDevs() const noexcept {
    return m_leadDevs;
};