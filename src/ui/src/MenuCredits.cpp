#include "../MenuCredits.hpp"

#include <Util.h>

#include <rhatt/prelude.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace cs::brkd::horrible::prelude;

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

// hardcoded license description for this build of horrible menu if web request fails
static auto constexpr g_defaultLicense =
    "The current build of [Horrible Menu](mod:cubicstudios.horriblemenu) is licensed under the [GNU General Public License v3.0 (GPL-3.0)](https://github.com/CubicCommunity/HorribleMenu/blob/main/LICENSE.md).\n\n"
    "Third-party assets, API modifications, libraries, and other external resources are credited in the credits interface.";

$on_mod(Loaded) {
    CreditsManager::get()->loadLeadDevs();
}

Result<LeadDevIcon> matjson::Serialize<LeadDevIcon>::fromJson(Value const& value) {
    LeadDevIcon out;

    GEODE_UNWRAP_INTO(out.id, value["id"].asString());
    GEODE_UNWRAP_INTO(out.name, value["name"].asString());
    GEODE_UNWRAP_INTO(out.account, value["account"].asInt());
    GEODE_UNWRAP_INTO(out.icon, value["icon"].asInt());
    GEODE_UNWRAP_INTO(out.color1, value["color1"].asInt());
    GEODE_UNWRAP_INTO(out.color2, value["color2"].asInt());
    GEODE_UNWRAP_INTO(out.glowColor, value["glowColor"].asInt());

    return Ok(std::move(out));
}

matjson::Value matjson::Serialize<LeadDevIcon>::toJson(LeadDevIcon const& value) {
    Value out;
    out["id"] = value.id;
    out["name"] = value.name;
    out["account"] = value.account;
    out["icon"] = value.icon;
    out["color1"] = value.color1;
    out["color2"] = value.color2;
    out["glowColor"] = value.glowColor;

    return out;
}

bool MenuPlayer::init(ZStringView name, int account, int icon, int color1, int color2, int glowColor) {
    if (!CCNode::init()) return false;

    auto layout = ColumnLayout::create()
                      ->setGap(2.5f)
                      ->setAutoGrowAxis(0.0f)
                      ->setAxisAlignment(AxisAlignment::Center)
                      ->setCrossAxisAlignment(AxisAlignment::Start)
                      ->setAutoScale(false);

    setLayout(layout);
    setAnchorPoint(cw::anchor::center);

    auto playerIcon = cue::PlayerIcon::create(IconType::Cube, icon, color1, color2, glowColor);

    addChild(playerIcon);

    auto labelBtn = Button::createWithLabel(
        name,
        cw::font::gold,
        [account](auto) {
            if (auto page = ProfilePage::create(account, false)) page->show();
        }
    );
    labelBtn->setID("profile-btn");
    labelBtn->setScale(0.375f);

    playerIcon->setPosition(getScaledContentSize() / 2.0f);
    labelBtn->setPosition({ getScaledContentWidth() / 2.0f, getScaledContentHeight() + (labelBtn->getScaledContentHeight() * 0.875f) });

    addChild(labelBtn, 1);

    updateLayout();

    return true;
}

MenuPlayer* MenuPlayer::create(ZStringView name, int account, int icon, int color1, int color2, int glowColor) {
    auto ret = new MenuPlayer();
    if (ret->init(name, account, icon, color1, color2, glowColor)) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
}

bool MenuCredits::init(ZStringView theme) {
    auto btns = themes::getCircleBaseColor(theme);

    if (!Popup::init(400.0f, 265.0f, themes::getBackgroundSprite(theme))) return false;

    setID("credits"_spr);
    setTitle("Credits");
    setCloseButtonSpr(themes::createThemeCircleSprite(btns));

    addSideArt(m_mainLayer, SideArt::All, SideArtStyle::PopupGold);

    auto leadDevLabel = Label::create("Lead Developers", cw::font::big);
    leadDevLabel->setID("lead-dev-label");
    leadDevLabel->setScale(0.425f);
    leadDevLabel->setAnchorPoint({ 0, 0.5 });
    leadDevLabel->setPosition({ 30.0f, (m_mainLayer->getScaledContentHeight() / 2.0f) + 91.25f });

    m_mainLayer->addChild(leadDevLabel);

    auto leadDevContainerLayout = RowLayout::create()
                                      ->setGap(5.0f)
                                      ->setAutoScale(false)
                                      ->setAutoGrowAxis(0.0f);

    auto leadDevContainer = CCNode::create();
    leadDevContainer->setID("lead-dev-container");
    leadDevContainer->setAnchorPoint({ cw::anchor::center });
    leadDevContainer->setLayout(leadDevContainerLayout);

    m_mainLayer->addChild(leadDevContainer, 1);

    for (auto const& dev : CreditsManager::get()->getLeadDevs()) {
        if (auto player = MenuPlayer::create(dev.name, dev.account, dev.icon, dev.color1, dev.color2, dev.glowColor)) {
            player->setID(dev.id);
            leadDevContainer->addChild(player);
        };
    };

    leadDevContainer->updateLayout();

    leadDevContainer->setPosition({ 2.5f + leadDevLabel->getPositionX() + (leadDevContainer->getScaledContentWidth() / 2.0f), leadDevLabel->getPositionY() - 35.0f });

    auto leadDevContainerBg = cue::createBackground(
        { leadDevContainer->getScaledContentWidth() + 10.0f, leadDevContainer->getScaledContentHeight() + 6.25f },
        {
            .zOrder = 0,
            .id = "",
        }
    );
    leadDevContainerBg->setPosition(leadDevContainer->getPosition());

    m_mainLayer->addChild(leadDevContainerBg);

    auto resrcBtnContainerLayout = RowLayout::create()
                                       ->setGap(2.5f)
                                       ->setAutoScale(false)
                                       ->setAxisAlignment(AxisAlignment::Even)
                                       ->setCrossAxisAlignment(AxisAlignment::Even)
                                       ->setGrowCrossAxis(true);

    auto resrcBtnContainer = CCNode::create();
    resrcBtnContainer->setID("resources-container");
    resrcBtnContainer->setAnchorPoint({ 0, 0.5 });
    resrcBtnContainer->setContentSize({ (m_mainLayer->getScaledContentWidth() - leadDevContainerBg->getScaledContentWidth()) - 60.0f, leadDevContainer->getScaledContentHeight() });
    resrcBtnContainer->setPosition({ leadDevContainer->getPositionX() + (leadDevContainerBg->getScaledContentWidth() / 2.0f) + 5.0f, leadDevContainer->getPositionY() });
    resrcBtnContainer->setLayout(resrcBtnContainerLayout);

    m_mainLayer->addChild(resrcBtnContainer, 9);

    auto resrcBtns = std::array{
        ResourceButton{
            "breakeode-support-btn",
            "Need Help?",
            [](auto) {
                cw::showAlert(
                    "Breakeode Support",
                    "If <cg>Horrible Menu</c> <cy>is causing issues</c>, you may reach out to the <cl>Breakeode</c> development team by <cy>creating a support ticket in the Discord server</c>. If you have any other questions, feel free to ask!\n\n"
                    "Would you like to join <cl>Breakeode</c>'s <cj>support Discord server</c>?",
                    "Cancel",
                    "OK",
                    [](auto, bool ok) {
                        if (ok) web::openLinkInBrowser("https://dsc.gg/breakeode");
                    }
                );
            },
        },
        ResourceButton{
            "changelog-btn",
            "What's New?",
            [](auto) {
                openChangelogPopup(mod);
            },
        },
    };

    for (auto& b : resrcBtns) {
        auto btn = Button::createWithNode(
            ButtonSprite::create(
                b.label.c_str(),
                cw::font::gold,
                themes::getButtonSquareSprite(theme)
            ),
            std::move(b.callback)
        );
        btn->setID(std::move(b.id));
        btn->setScale(0.625f);

        resrcBtnContainer->addChild(btn);
    };

    resrcBtnContainer->updateLayout();

    auto creditsMd = MDTextArea::create(
        "# ![🛠](frame:GJ_hammerIcon_001.png?scale=0.875) Resources\n"
        "**[Geode Team](mod:geode.loader)**: '*[Geode SDK](https://github.com/geode-sdk/geode)*' Geometry Dash modding framework\n\n"
        "**[Presta](user:540196)**: '*[Congregation](level:68668045)*' jumpscare level\n\n"
        "**[IcEDCave](user:1504161)**: '*[Grief](level:149992717)*' jumpscare level\n\n"
        "**[sink](user:5871590)**: '*[Tidal Wave](level:93733469)*' jumpscare level\n\n"
        "**[jackaezie](user:9456524)**: '*[YOUVE BEEN TROLLED](level:57436521)*' jumpscare level\n\n"
        "**[alk1m123](user:11535118)**: '*[Sapphire SDK](https://x.com/GeodeSDK/status/2039225279353176398/)*' logo\n\n"
        "**[Uproxide](user:25397826)**: '*The Yellow One*' sprite from [More Difficulties](mod:uproxide.more_difficulties)\n\n<mod:uproxide.more_difficulties>\n\n"
        "**[Cheeseworks](user:6408873)**: [Mod Developer Branding](mod:cheeseworks.moddevbranding) image for this mod\n\n<mod:cheeseworks.moddevbranding>\n\n"
        "**[Erymanthus](user:1941705)**: His socials' profile picture\n\n"
        "**[dankmeme](user:9735891)**: ''*[cue](https://github.com/dankmeme01/cue)*' user interface library, '*[asp2](https://github.com/dankmeme01/asp2)*' utility library\n\n"
        "# ![💝](frame:GJ_diamondsIcon_001.png?scale=0.875) Special Thanks\n"
        "**[Cheeseworks](user:6408873)**: Internals, API/DX, UI/UX, options' features, mod branding\n\n"
        "**[ArcticWoof](user:7689052)**: UI, options' features, Horrible Menu logo, original idea for this mod\n\n"
        "**[Team Avalanche](user:31079132)**: Supporting the project since its experimental days\n\n"
        "**[Talindrusk](user:30689066)**: Codebase contributions, playtesting",
        {
            m_mainLayer->getScaledContentWidth() - 55.0f,
            140.0f,
        }
    );
    creditsMd->setID("credits");
    creditsMd->setZOrder(1);
    creditsMd->setPosition({ m_mainLayer->getScaledContentWidth() / 2.0f, 90.0f });

    m_mainLayer->addChildAtPosition(creditsMd, Anchor::Bottom, { 0.0f, 90.0f });

    auto websiteBtn = Button::createWithNode(
        ButtonSprite::create(
            "Website",
            cw::font::big,
            themes::getButtonSquareSprite(theme)
        ),
        [](auto) {
            cw::showAlert(
                "Breakeode",
                "Visit <cl>Breakeode</c>'s official website?",
                "Cancel",
                "OK",
                [](auto, bool ok) {
                    if (ok) web::openLinkInBrowser("https://breakeode.cubicstudios.xyz/");
                }
            );
        }
    );
    websiteBtn->setID("website-btn");
    websiteBtn->setScale(0.625f);
    websiteBtn->setPosition({ m_mainLayer->getScaledContentWidth() / 2.0f, 0.0f });

    m_mainLayer->addChild(websiteBtn, 1);

    // 1% chance to show sapphire logo over geode
    auto isModBtnSecret = cw::rng::fast() <= 1;

    auto modBtn = Button::createWithNode(
        CircleButtonSprite::createWithSpriteFrameName(
            isModBtnSecret ? themes::getIconSprite(themes::icons::SapphireSDK) : "geode-logo-outline-gold.png"_gode,
            isModBtnSecret ? 1.125f : 1.0f,
            btns
        ),
        [](auto) {
            openInfoPopup(mod);
        }
    );
    modBtn->setID("mod-information-btn");
    modBtn->setScale(0.5f);
    modBtn->setPosition({ 15.0f, 15.0f });

    m_mainLayer->addChild(modBtn, 9);

    auto om = OptionManager::get();

    auto integrations = om->getMods();
    auto str = (integrations.size() > 0) ? fmt::format("Here's the <cg>full list of your installed mods (**`{}`**)</c> which <cy>currently add their own features as options to [Horrible Menu](mod:cubicstudios.horriblemenu)</c>.\n\n---", integrations.size()) : "If <cg>different mods</c> use this mod's API to <cy>add their own options</c>, they will all be listed here.";

    if (integrations.size() > 0) {
        for (auto const& i : integrations) {
            log::trace("Listing mod {}", i->getID());
            str = fmt::format("{}\n\n<mod:{}>", str, i->getID());
        };
    };

    auto integrationsBtn = Button::createWithNode(
        CircleButtonSprite::createWithSpriteFrameName(
            "grid-view.png"_gode,
            0.75f,
            btns
        ),
        [integrations = std::move(integrations), str = std::move(str)](auto) {
            if (integrations.size() > 0) {
                MDPopup::create(
                    "Integrations",
                    str,
                    "OK"
                )
                    ->show();
            } else {
                cw::showAlert(
                    "Integrations",
                    str,
                    "OK",
                    nullptr,
                    nullptr
                );
            };
        }
    );
    integrationsBtn->setID("integrations-btn");
    integrationsBtn->setScale(0.5f);
    integrationsBtn->setPosition({ m_mainLayer->getScaledContentWidth() - 15.0f, 15.0f });

    m_mainLayer->addChild(integrationsBtn, 9);

    cw::addHelpButton(
        m_mainLayer,
        "This menu aims to give credit to everyone who has <cy>contributed to the development of Horrible Menu</c>, directly or indirectly.\n\n"
        "<co>If we missed anyone, let us know by opening an issue about it on our GitHub repository!</c>\n\n"
        "<cd>Thanks to everyone who has helped this project in any way! We greatly appreciate you! <3</c>"
    );

    return true;
}

MenuCredits* MenuCredits::create(ZStringView theme) {
    auto ret = new MenuCredits();
    if (ret->init(theme)) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
}

void CreditsManager::loadLeadDevs() {
    log::trace("Requested to fetch data on lead developers");

#define LEAD_DEVS_INTERNAL(container, devRes)                     \
    if (devRes.isErr()) {                                         \
        log::error("{}", devRes.unwrapErr());                     \
        continue;                                                 \
    };                                                            \
                                                                  \
    auto dev = std::move(devRes).unwrap();                        \
                                                                  \
    log::trace("Pushing {} to lead developer list...", dev.name); \
    container.push_back(std::move(dev))

#define LEAD_DEVS(container, array)                                \
    for (auto const& d : array) {                                  \
        LEAD_DEVS_INTERNAL(container, Result<LeadDevIcon>(Ok(d))); \
    }

    if (m_leadDevs.size() <= 0) {
        log::debug("Sending web request for lead developer credits");

        m_creditsTask.spawn(
            cw::request::base().get("https://api.cubicstudios.xyz/breakeode/v1/horrible/credits"),
            [this](web::WebResponse res) {
                auto const fallback = [this](std::string_view err = "") {
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

                log::info("Successfully fetched lead developer list");
                for (auto const& val : array) {
                    LEAD_DEVS_INTERNAL(m_leadDevs, val.as<LeadDevIcon>());
                };
            }
        );
    } else {
        log::error("Lead developer data already populated");
    };
}

std::span<const LeadDevIcon> CreditsManager::getLeadDevs() const noexcept {
    return m_leadDevs;
}