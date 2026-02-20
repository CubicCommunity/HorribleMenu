#include "../OptionItem.hpp"

#include <Utils.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class OptionItem::Impl final {
public:
    bool s_compatible = false; // If this option is compatible with the current platform

    // The option
    Option m_option = {
        "unk"_spr,
        "Unknown Option",
        "No description provided.",
        "General"
    };

    CCMenuItemToggler* m_toggler = nullptr; // The toggler for the option
};

OptionItem::OptionItem() {
    m_impl = std::make_unique<Impl>();
};

OptionItem::~OptionItem() {};

bool OptionItem::init(CCSize const& size, Option option) {
    m_impl->m_option = std::move(option);

    // check for compatibility
    for (auto const& p : m_impl->m_option.platforms) {
        if (p & GEODE_PLATFORM_TARGET) {
            m_impl->s_compatible = true;
            break;
        };
    };

    if (!CCMenu::init()) return false;

    setID(m_impl->m_option.id);
    setContentSize(size);
    setAnchorPoint({ 0.5, 1 });

    auto bg = CCScale9Sprite::create("square02_001.png");
    bg->setID("background");
    bg->setScale(0.5f);
    bg->setAnchorPoint({ 0, 0 });
    bg->setContentSize(getScaledContentSize() * 2.f);
    bg->setOpacity(40);

    addChild(bg, -1);

    // Horizontal layout: [toggle] [name] [info]
    float padding = 5.f;
    float yCenter = getScaledContentHeight() / 2.f;

    float x = padding;

    auto togglerOff = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    togglerOff->setScale(0.875f);
    auto togglerOn = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    togglerOn->setScale(0.875f);

    // toggler for the option
    m_impl->m_toggler = CCMenuItemToggler::create(
        togglerOff,
        togglerOn,
        this,
        menu_selector(OptionItem::onToggle)
    );
    m_impl->m_toggler->setID("toggler");
    m_impl->m_toggler->setAnchorPoint({ 0.5f, 0.5f });
    m_impl->m_toggler->setPosition({ x + 12.f, yCenter });
    m_impl->m_toggler->setScale(0.875f);

    // Set toggler state based on saved mod option value
    if (horribleMod) m_impl->m_toggler->toggle(horribleMod->getSavedValue<bool>(m_impl->m_option.id));

    addChild(m_impl->m_toggler);

    x += 30.f;

    // name of the joke
    auto nameLabel = CCLabelBMFont::create(
        m_impl->m_option.name.c_str(),
        "bigFont.fnt",
        getScaledContentWidth() - 40.f,
        kCCTextAlignmentLeft
    );
    nameLabel->setID("name-label");
    nameLabel->setLineBreakWithoutSpace(true);
    nameLabel->setAnchorPoint({ 0.f, 0.5f });
    nameLabel->setPosition({ x, yCenter });
    nameLabel->setScale(0.4f);

    auto categoryLabel = CCLabelBMFont::create(
        m_impl->m_option.category.c_str(),
        "goldFont.fnt",
        getScaledContentWidth() - 60.f,
        kCCTextAlignmentLeft
    );
    categoryLabel->setID("category-label");
    categoryLabel->setLineBreakWithoutSpace(true);
    categoryLabel->setAnchorPoint({ 0.f, 0.5f });
    categoryLabel->setPosition({ x, yCenter + 10.f });
    categoryLabel->setOpacity(200);
    categoryLabel->setScale(0.25f);

    // Set color based on m_impl->m_option.Tier
    switch (m_impl->m_option.silly) {
    case SillyTier::Low: // green
        nameLabel->setColor(colors::green);
        break;

    case SillyTier::Medium: // yellow
        nameLabel->setColor(colors::yellow);
        break;

    case SillyTier::High: // red
        nameLabel->setColor(colors::red);
        break;

    default: // white
        nameLabel->setColor(colors::white);
        break;
    };

    addChild(nameLabel);
    addChild(categoryLabel);

    if (horribleMod->getSettingValue<bool>("dev-mode")) {
        auto idLabel = CCLabelBMFont::create(
            m_impl->m_option.id.c_str(),
            "chatFont.fnt",
            getScaledContentWidth() - 20.f,
            kCCTextAlignmentLeft
        );
        idLabel->setID("id-label");
        idLabel->setLineBreakWithoutSpace(true);
        idLabel->setPosition({ x, yCenter - 10.f });
        idLabel->setAnchorPoint({ 0.f, 0.5f });
        idLabel->setColor(colors::black);
        idLabel->setOpacity(125);
        idLabel->setScale(0.5f);

        addChild(idLabel);
    };

    x += nameLabel->getScaledContentWidth() + 15.f;

    auto helpBtnSprite = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    helpBtnSprite->setScale(0.75f);

    // info button
    auto helpBtn = CCMenuItemSpriteExtra::create(
        helpBtnSprite,
        this,
        menu_selector(OptionItem::onDescription)
    );
    helpBtn->setID("help-btn");
    helpBtn->setAnchorPoint({ 0.5f, 0.5f });
    helpBtn->setPosition({ getScaledContentWidth() - padding - 10.f, yCenter });

    addChild(helpBtn);

    if (!m_impl->s_compatible) {
        m_impl->m_toggler->toggle(false);

        togglerOn->setDisplayFrame(togglerOff->displayFrame());

        togglerOff->setColor(colors::gray);
        togglerOn->setColor(colors::gray);

        bg->setColor(colors::gray);

        nameLabel->setColor(colors::gray);
        categoryLabel->setColor(colors::gray);

        auto newHelpBtnSprite = CCSprite::createWithSpriteFrameName("geode.loader/info-alert.png");
        newHelpBtnSprite->setScale(0.75f);
        
        helpBtn->setSprite(newHelpBtnSprite);

        saveTogglerState();
    };

    return true;
};

void OptionItem::saveTogglerState() {
    if (m_impl->m_toggler) options::set(m_impl->m_option.id, m_impl->m_toggler->isToggled());
};

void OptionItem::onToggle(CCObject*) {
    if (m_impl->s_compatible) {
        saveTogglerState();
        if (m_impl->m_option.restart) {
            Notification::create("Restart required!", NotificationIcon::Warning, 2.5f)->show();
            log::warn("Restart required to apply option {}", m_impl->m_option.id);
        };

        auto now = options::get(m_impl->m_option.id);

        OptionEvent(m_impl->m_option.id).send(m_impl->m_option.id, now);

        log::info("Option {} now set to {}", m_impl->m_option.name, now ? "disabled" : "enabled"); // wtf is it other way around lmao
    } else if (m_impl->m_toggler) {
        Notification::create(fmt::format("{} is unavailable for {}", m_impl->m_option.name, GEODE_PLATFORM_NAME), NotificationIcon::Error, 1.25f)->show();
        log::error("Option {} is not available for platform {}", m_impl->m_option.id, GEODE_PLATFORM_SHORT_IDENTIFIER);

        m_impl->m_toggler->toggle(false);
    };
};

void OptionItem::onDescription(CCObject*) {
    if (auto popup = FLAlertLayer::create(
        m_impl->m_option.name.c_str(),
        m_impl->m_option.description.c_str(),
        "OK"
    )) popup->show();
};

void OptionItem::onExit() {
    saveTogglerState();
    CCMenu::onExit();
};

Option OptionItem::getOption() const noexcept {
    return m_impl->m_option;
};

bool OptionItem::isCompatible() const noexcept {
    return m_impl->s_compatible;
};

OptionItem* OptionItem::create(CCSize const& size, Option option) {
    auto ret = new OptionItem();
    if (ret->init(size, std::move(option))) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
}; 