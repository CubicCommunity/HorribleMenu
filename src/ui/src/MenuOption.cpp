#include "../MenuOption.h"

#include <Utils.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class MenuOption::Impl final {
public:
    bool compatible = false;  // If this option is compatible with the current platform

    std::weak_ptr<Option> option;  // A view into the option value :)

    bool hasInternet = false;  // would rather call doWeHaveInternet once

    CCMenuItemToggler* toggler = nullptr;  // The toggler for the option
    CCNode* newContainer = nullptr;        // Container for the "New!" label and icon

    Callback pinCallback = nullptr;  // Callback for when the option is pinned or unpinned

    // Save the current state of the toggler as the option state
    void saveTogglerState() {
        if (auto o = option.lock()) {
            auto saved = options::get(o->getID());
            if (toggler) options::set(o->getID(), toggler->isToggled(), saved.pin, saved.viewed);
        };

        hideNewLabel();
    };

    // Notify the user if this option is not compatible for their current platform
    void notifyIncompats() {
        if (auto o = option.lock()) {
            if (compatible) {
                if (!(o->isOnline() ? hasInternet : true)) {  // woah evil gay ternaries !!!
                    log::warn("Option {} requires a working internet connection to function", o->getID());
                    if (auto notif = Notification::create(fmt::format("{} needs internet to work properly", o->getName()), NotificationIcon::Warning, 2.5f)) notif->show();
                };
            } else {
                log::warn("Option {} is unavailable for platform {}", o->getID(), GEODE_PLATFORM_SHORT_IDENTIFIER);
                if (auto notif = Notification::create(fmt::format("{} is not available for {}", o->getName(), GEODE_PLATFORM_NAME), NotificationIcon::Error, 1.25f)) notif->show();
            };
        };
    };

    void hideNewLabel() {
        if (auto label = WeakRef(newContainer).lock()) label->setVisible(false);
    };

    constexpr auto getTierDescString(SillyTier silly, bool compat) noexcept {
        if (!compat) return "<cp>OPTION UNAVAILABLE</c>";

        switch (silly) {
            default: [[fallthrough]];

            case SillyTier::Low: return "<cg>Silly Tier - LOW</c>";
            case SillyTier::Medium: return "<cy>Silly Tier - MEDIUM</c>";
            case SillyTier::High: return "<cr>Silly Tier - HIGH</c>";
            case SillyTier::None: return "Silly Tier - UNKNOWN";
        };
    };
};

MenuOption::MenuOption() : m_impl(std::make_unique<Impl>()) {};
MenuOption::~MenuOption() {};

bool MenuOption::init(CCSize const& size, std::weak_ptr<Option> option, ZStringView theme, bool devMode, bool hasInternet) {
    m_impl->option = std::move(option);
    m_impl->hasInternet = hasInternet;

    auto o = m_impl->option.lock();

    // check for compatibility
    for (auto const& p : o->getSupportedPlatforms()) {
        if (p & GEODE_PLATFORM_TARGET) {
            m_impl->compatible = true;
            break;
        };
    };

    if (!CCMenu::init()) return false;

    setID(o->getID());
    setContentSize(size);
    setAnchorPoint({0.5, 1});

    auto bg = NineSlice::create(themes::square);
    bg->setID("background");
    bg->setAnchorPoint({0, 0});
    bg->setContentSize(getScaledContentSize());
    bg->setScaleMultiplier(0.5f);
    bg->setOpacity(50);

    addChild(bg, -1);

    // Horizontal layout: [toggle] [name] [info]
    float yCenter = getScaledContentHeight() / 2.f;

    auto x = 5.f;

    auto togglerOff = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    togglerOff->setScale(0.875f);
    auto togglerOn = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    togglerOn->setScale(0.875f);

    // toggler for the option
    m_impl->toggler = CCMenuItemToggler::create(togglerOff, togglerOn, this, menu_selector(MenuOption::onToggle));
    m_impl->toggler->setID("toggler");
    m_impl->toggler->setAnchorPoint({0.5f, 0.5f});
    m_impl->toggler->setPosition({x + 12.f, yCenter});
    m_impl->toggler->setScale(0.875f);

    // Set toggler state based on saved mod option value
    m_impl->toggler->toggle(options::isEnabled(o->getID()));

    addChild(m_impl->toggler);

    x += 30.f;

    // name of the joke
    auto nameLabel = CCLabelBMFont::create(o->getName().c_str(), "bigFont.fnt", getScaledContentWidth() - 40.f, kCCTextAlignmentLeft);
    nameLabel->setID("name-label");
    nameLabel->setLineBreakWithoutSpace(true);
    nameLabel->setAnchorPoint({0.f, 0.5f});
    nameLabel->setPosition({x, yCenter});
    nameLabel->setScale(0.4f);

    auto categoryLabel = CCLabelBMFont::create(o->getCategory().c_str(), "goldFont.fnt", getScaledContentWidth() - 60.f, kCCTextAlignmentLeft);
    categoryLabel->setID("category-label");
    categoryLabel->setLineBreakWithoutSpace(true);
    categoryLabel->setAnchorPoint({0.f, 0.5f});
    categoryLabel->setPosition({x, yCenter + 10.f});
    categoryLabel->setOpacity(200);
    categoryLabel->setScale(0.25f);

    // Set color based on tier
    switch (o->getSillyTier()) {
        default:  // white
            nameLabel->setColor(colors::white);
            break;

        case SillyTier::Low:  // green
            nameLabel->setColor(colors::green);
            break;

        case SillyTier::Medium:  // yellow
            nameLabel->setColor(colors::yellow);
            break;

        case SillyTier::High:  // red
            nameLabel->setColor(colors::red);
            break;
    };

    addChild(nameLabel);
    addChild(categoryLabel);

    auto menuLayout = RowLayout::create()
                          ->setGap(5.f)
                          ->setAutoScale(false)
                          ->setAxisReverse(true)
                          ->setAutoGrowAxis(0.f);

    auto menu = CCMenu::create();
    menu->setID("menu");
    menu->setAnchorPoint({1, 0.5});
    menu->setContentSize({0.f, 0.f});
    menu->setPosition({getScaledContentWidth() - 7.5f, yCenter});
    menu->setLayout(menuLayout);

    addChild(menu);

    auto onlineCompat = (o->isOnline() ? m_impl->hasInternet : true);

    // info button
    auto infoBtn = Button::createWithSpriteFrameName(
        (m_impl->compatible) ? (onlineCompat ? "GJ_infoIcon_001.png" : "geode.loader/info-warning.png") : "geode.loader/info-alert.png",
        [this](auto) {
            if (auto o = m_impl->option.lock()) {
                auto formatDesc = fmt::format("{}\n\n{}{}", (o->getDescription().size() > 0) ? o->getDescription() : "<cc>No description provided.</c>", o->isOnline() ? "<co>An internet connection is required.</c>\n" : "", m_impl->getTierDescString(o->getSillyTier(), m_impl->compatible));

                createQuickPopup(
                    o->getName().c_str(),
                    std::move(formatDesc),
                    "OK",
                    nullptr,
                    375.f,
                    nullptr);

                auto saved = options::get(o->getID());
                if (!saved.viewed) {
                    options::set(o->getID(), saved.enabled, saved.pin, true);
                    m_impl->hideNewLabel();
                };
            };

            m_impl->notifyIncompats();
        });
    infoBtn->setID("info-btn");
    infoBtn->setScale(onlineCompat ? 0.75f : 0.625f);

    menu->addChild(infoBtn);

    auto pinOff = CCSprite::createWithSpriteFrameName("geode.loader/pin.png");
    pinOff->setScale(0.425f);
    pinOff->setOpacity(75);
    auto pinOn = CCSprite::createWithSpriteFrameName("geode.loader/pin.png");
    pinOn->setScale(0.425f);
    pinOn->setOpacity(225);

    pinOff->setBlendFunc({GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA});
    pinOn->setColor(themes::getColor(theme));

    auto pinBtn = CCMenuItemToggler::create(pinOff, pinOn, this, menu_selector(MenuOption::onPin));
    pinBtn->setID("pin-btn");

    pinBtn->toggle(options::isPinned(o->getID()));

    menu->addChild(pinBtn);

    menu->updateLayout();

    auto newContainerLayout = RowLayout::create()
                                  ->setGap(1.25f)
                                  ->setAutoScale(false)
                                  ->setAutoGrowAxis(0.f);

    m_impl->newContainer = CCNode::create();
    m_impl->newContainer->setID("new-option-container");
    m_impl->newContainer->setAnchorPoint({0, 0.5});
    m_impl->newContainer->setPosition({x, 5.25f});
    m_impl->newContainer->setLayout(newContainerLayout);

    addChild(m_impl->newContainer, 9);

    auto newIcon = CCSprite::createWithSpriteFrameName("geode.loader/updates-available.png");
    newIcon->setID("new-option-icon");
    newIcon->setScale(0.25f);

    auto newLabel = CCLabelBMFont::create("New!", "bigFont.fnt");
    newLabel->setID("new-option-label");
    newLabel->setScale(0.25f);
    newLabel->setColor(colors::cyan);

    m_impl->newContainer->addChild(newIcon);
    m_impl->newContainer->addChild(newLabel);

    m_impl->newContainer->updateLayout();

    m_impl->newContainer->setVisible(!options::isViewed(o->getID()));
    m_impl->newContainer->setScale(0.75f);

    if (!m_impl->compatible) {
        m_impl->toggler->toggle(false);

        togglerOn->setDisplayFrame(togglerOff->displayFrame());

        togglerOff->setColor(colors::gray);
        togglerOn->setColor(colors::gray);

        bg->setColor(colors::gray);

        nameLabel->setColor(colors::gray);
        categoryLabel->setColor(colors::gray);

        m_impl->saveTogglerState();
    };

    if (devMode) {
        auto str = fmt::format("{} | {} delegate(s)", o->getID(), options::getDelegates(o->getID()));

        auto idLabel = CCLabelBMFont::create(str.c_str(), "chatFont.fnt", getScaledContentWidth() - 20.f, kCCTextAlignmentLeft);
        idLabel->setID("id-label");
        idLabel->setLineBreakWithoutSpace(true);
        idLabel->setPosition({getScaledContentWidth() - 7.5f, 5.25f});
        idLabel->setAnchorPoint({1, 0.5});
        idLabel->setColor(colors::black);
        idLabel->setOpacity(125);
        idLabel->setScale(0.5f);

        addChild(idLabel);
    };

    return true;
};

void MenuOption::onToggle(CCObject*) {
    if (m_impl->toggler && m_impl->compatible) {
        auto now = !m_impl->toggler->isToggled();

        if (auto o = m_impl->option.lock()) {
            auto saved = options::get(o->getID());
            options::set(o->getID(), now, saved.pin, true);

            if (o->isRestartRequired()) {
                Notification::create("Restart required!", NotificationIcon::Warning, 2.5f)->show();
                log::warn("Restart required to apply option {}", o->getID());
            };

            log::info("Option {} now set to {}", o->getName(), now ? "enabled" : "disabled");
        };

        m_impl->hideNewLabel();
    } else if (m_impl->toggler) {
        m_impl->toggler->toggle(false);
    };

    m_impl->notifyIncompats();
};

void MenuOption::onPin(CCObject* sender) {
    if (auto pinBtn = typeinfo_cast<CCMenuItemToggler*>(sender)) {
        if (auto o = m_impl->option.lock()) options::set(o->getID(), options::isEnabled(o->getID()), !pinBtn->isToggled(), true);

        if (m_impl->pinCallback) m_impl->pinCallback();
        m_impl->hideNewLabel();
    };
};

void MenuOption::setPinCallback(Callback&& callback) & {
    m_impl->pinCallback = std::move(callback);
};

std::weak_ptr<Option> const& MenuOption::getOption() const noexcept {
    return m_impl->option;
};

bool MenuOption::isCompatible() const noexcept {
    return m_impl->compatible;
};

MenuOption* MenuOption::create(CCSize const& size, std::weak_ptr<Option> option, ZStringView theme, bool devMode, bool hasInternet) {
    auto ret = new MenuOption();
    if (ret->init(size, std::move(option), theme, devMode, hasInternet)) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};