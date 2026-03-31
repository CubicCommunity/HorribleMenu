#include "../OptionItem.h"

#include <Utils.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class OptionItem::Impl final {
public:
    bool compatible = false;  // If this option is compatible with the current platform

    Option option;  // A copy of the option

    CCMenuItemToggler* toggler = nullptr;  // The toggler for the option
    CCNode* newContainer = nullptr;        // Container for the "New!" label and icon

    Callback pinCallback = nullptr;  // Callback for when the option is pinned or unpinned

    // Save the current state of the toggler as the option state
    void saveTogglerState() {
        auto saved = options::get(option.getID());
        if (toggler) options::set(option.getID(), toggler->isToggled(), saved.pin, saved.viewed);

        clearNewLabel();
    };

    // Notify the user if this option is not compatible for their current platform
    void notifyIncompat() {
        if (!compatible) {
            log::warn("Option {} is not available for platform {}", option.getID(), GEODE_PLATFORM_SHORT_IDENTIFIER);
            if (auto notif = Notification::create(fmt::format("{} is unavailable for {}", option.getName(), GEODE_PLATFORM_NAME), NotificationIcon::Error, 1.25f)) notif->show();
        };
    };

    void clearNewLabel() {
        if (auto label = WeakRef(newContainer).lock()) label->setVisible(false);
    };

    constexpr const char* getTierDescString(SillyTier silly, bool compat) noexcept {
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

OptionItem::OptionItem() : m_impl(std::make_unique<Impl>()) {};
OptionItem::~OptionItem() {};

bool OptionItem::init(CCSize const& size, Option option, bool devMode) {
    m_impl->option = std::move(option);

    // check for compatibility
    for (auto const& p : m_impl->option.getSupportedPlatforms()) {
        if (p & GEODE_PLATFORM_TARGET) {
            m_impl->compatible = true;
            break;
        };
    };

    if (!CCMenu::init()) return false;

    setID(m_impl->option.getID());
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
    m_impl->toggler = CCMenuItemToggler::create(togglerOff, togglerOn, this, menu_selector(OptionItem::onToggle));
    m_impl->toggler->setID("toggler");
    m_impl->toggler->setAnchorPoint({0.5f, 0.5f});
    m_impl->toggler->setPosition({x + 12.f, yCenter});
    m_impl->toggler->setScale(0.875f);

    // Set toggler state based on saved mod option value
    m_impl->toggler->toggle(options::isEnabled(m_impl->option.getID()));

    addChild(m_impl->toggler);

    x += 30.f;

    // name of the joke
    auto nameLabel = CCLabelBMFont::create(m_impl->option.getName().c_str(), "bigFont.fnt", getScaledContentWidth() - 40.f, kCCTextAlignmentLeft);
    nameLabel->setID("name-label");
    nameLabel->setLineBreakWithoutSpace(true);
    nameLabel->setAnchorPoint({0.f, 0.5f});
    nameLabel->setPosition({x, yCenter});
    nameLabel->setScale(0.4f);

    auto categoryLabel = CCLabelBMFont::create(m_impl->option.getCategory().c_str(), "goldFont.fnt", getScaledContentWidth() - 60.f, kCCTextAlignmentLeft);
    categoryLabel->setID("category-label");
    categoryLabel->setLineBreakWithoutSpace(true);
    categoryLabel->setAnchorPoint({0.f, 0.5f});
    categoryLabel->setPosition({x, yCenter + 10.f});
    categoryLabel->setOpacity(200);
    categoryLabel->setScale(0.25f);

    // Set color based on m_impl->option.Tier
    switch (m_impl->option.getSillyTier()) {
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
                          ->setAxisReverse(true)
                          ->setAutoGrowAxis(0.f);

    auto menu = CCMenu::create();
    menu->setID("menu");
    menu->setAnchorPoint({1, 0.5});
    menu->setContentSize({0.f, 0.f});
    menu->setPosition({getScaledContentWidth() - 7.5f, yCenter});
    menu->setLayout(menuLayout);

    addChild(menu);

    // info button
    auto infoBtn = Button::createWithSpriteFrameName(
        m_impl->compatible ? "GJ_infoIcon_001.png" : "geode.loader/info-alert.png",
        [this](Button*) {
            m_impl->notifyIncompat();

            auto formatDesc = fmt::format("{}\n\n{}", (m_impl->option.getDescription().size() > 0) ? m_impl->option.getDescription() : "<cc>No description provided.</c>", m_impl->getTierDescString(m_impl->option.getSillyTier(), m_impl->compatible));

            if (auto popup = FLAlertLayer::create(
                    this,
                    m_impl->option.getName().c_str(),
                    std::move(formatDesc),
                    "OK",
                    nullptr,
                    375.f)) popup->show();

            auto saved = options::get(m_impl->option.getID());
            if (!saved.viewed) {
                options::set(m_impl->option.getID(), saved.enabled, saved.pin, true);
                m_impl->clearNewLabel();
            };
        });
    infoBtn->setID("info-btn");
    infoBtn->setScale(0.625f);

    menu->addChild(infoBtn);

    // @geode-ignore(unknown-resource)
    auto pinOff = CCSprite::createWithSpriteFrameName("geode.loader/pin.png");
    pinOff->setScale(0.5f);
    pinOff->setOpacity(75);
    // @geode-ignore(unknown-resource)
    auto pinOn = CCSprite::createWithSpriteFrameName("geode.loader/pin.png");
    pinOn->setScale(0.5f);
    pinOn->setOpacity(225);

    pinOff->setBlendFunc({GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA});
    pinOn->setColor(themes::getColor(thisMod->getSettingValue<std::string>("theme")));

    auto pinBtn = CCMenuItemToggler::create(pinOff, pinOn, this, menu_selector(OptionItem::onPin));
    pinBtn->setID("pin-btn");

    pinBtn->toggle(options::isPinned(m_impl->option.getID()));

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

    m_impl->newContainer->setVisible(!options::isViewed(m_impl->option.getID()));
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
        auto str = fmt::format("{} | {} delegate(s)", m_impl->option.getID(), options::getDelegates(m_impl->option.getID()));

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

void OptionItem::onToggle(CCObject*) {
    if (m_impl->toggler && m_impl->compatible) {
        auto now = !m_impl->toggler->isToggled();

        auto saved = options::get(m_impl->option.getID());
        options::set(m_impl->option.getID(), now, saved.pin, true);

        if (m_impl->option.isRestartRequired()) {
            Notification::create("Restart required!", NotificationIcon::Warning, 2.5f)->show();
            log::warn("Restart required to apply option {}", m_impl->option.getID());
        };

        log::info("Option {} now set to {}", m_impl->option.getName(), now ? "enabled" : "disabled");

        m_impl->clearNewLabel();
    } else if (m_impl->toggler) {
        m_impl->notifyIncompat();

        m_impl->toggler->toggle(false);
    };
};

void OptionItem::onPin(CCObject* sender) {
    if (auto pinBtn = typeinfo_cast<CCMenuItemToggler*>(sender)) {
        options::set(m_impl->option.getID(), options::isEnabled(m_impl->option.getID()), !pinBtn->isToggled(), true);

        if (m_impl->pinCallback) m_impl->pinCallback();
        m_impl->clearNewLabel();
    };
};

void OptionItem::setPinCallback(Callback&& callback) {
    m_impl->pinCallback = std::move(callback);
};

Option const& OptionItem::getOption() const noexcept {
    return m_impl->option;
};

bool OptionItem::isCompatible() const noexcept {
    return m_impl->compatible;
};

OptionItem* OptionItem::create(CCSize const& size, Option option, bool devMode) {
    auto ret = new OptionItem();
    if (ret->init(size, std::move(option), devMode)) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};