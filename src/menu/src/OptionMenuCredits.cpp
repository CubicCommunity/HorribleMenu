#include "../OptionMenuCredits.h"

#include <Utils.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

bool OptionMenuPlayer::init(ZStringView name, int account, int icon, ccColor3B const& color1, ccColor3B const& color2, ccColor3B const& glowColor) {
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

OptionMenuPlayer* OptionMenuPlayer::create(ZStringView name, int account, int icon, ccColor3B const& color1, ccColor3B const& color2, ccColor3B const& glowColor) {
    auto ret = new OptionMenuPlayer();
    if (ret->init(name, account, icon, color1, color2, glowColor)) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};

OptionMenuCredits* OptionMenuCredits::s_inst = nullptr;

bool OptionMenuCredits::init(ZStringView theme) {
    auto btns = themes::getCircleBaseColor(theme);

    if (!Popup::init(400.f, 265.f, themes::getBackgroundSprite(theme))) return false;

    setID("credits"_spr);
    setTitle("Credits");
    setCloseButtonSpr(CircleButtonSprite::createWithSpriteFrameName("geode.loader/close.png", 0.875f, btns, CircleBaseSize::Small));

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
            "arcticwoof",
            "ArcticWoof",
            7689052,
            290,
            {160, 255, 255},
            {255, 255, 255},
            {0, 255, 255},
        },
        {
            "cheeseworks",
            "Cheeseworks",
            6408873,
            28,
            {64, 64, 64},
            {252, 181, 255},
            {255, 255, 255},
        },
    };

    for (auto const& dev : devs) {
        if (auto player = OptionMenuPlayer::create(dev.name, dev.account, dev.icon, dev.color1, dev.color2, dev.glowColor)) leadDevContainer->addChild(player);
    };

    m_mainLayer->addChild(leadDevContainer);
    leadDevContainer->updateLayout();

    auto creditsMd = MDTextArea::create(
        "# ![🛠](frame:GJ_hammerIcon_001.png?scale=0.875) Additional Resources\n"
        "**[alk1m123](user:11535118)**: 'Sapphire SDK' logo\n\n"
        "**[Uproxide](user:25397826)**: 'The Yellow One' sprite from [More Difficulties](mod:uproxide.more_difficulties)\n\n<mod:uproxide.more_difficulties>\n\n"
        "**[Cheeseworks](user:6408873)**: [Mod Developer Branding](mod:cheeseworks.moddevbranding) image for Horrible Menu\n\n<mod:cheeseworks.moddevbranding>\n\n",
        {m_mainLayer->getScaledContentWidth() - 50.f,
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