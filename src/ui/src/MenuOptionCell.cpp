#include "../MenuOptionCell.hpp"

#include <Util.h>

#include <rhatt/prelude.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace cs::brkd::horrible::prelude;

bool MenuNothingNode::init(CCSize const& size, CCPoint const& pos) {
    if (!CCNode::init()) return false;

    setID("nothing-label");
    setPosition(pos);
    setContentSize(size);
    setAnchorPoint(cw::anchor::center);
    setVisible(false);

    auto label = Label::create("No options found :(", cw::font::big);
    label->setScale(0.5f);
    label->setOpacity(250);
    label->setAnchorPoint({ 0.5, 0 });
    label->setAlignment(Label::Alignment::Center);
    label->setLimitLabelWidth(getScaledContentWidth() * 0.875f, label->getScale(), label->getScale() * 0.125f);
    label->setPosition(getScaledContentSize() / 2.0f);

    addChild(label, 1);

    auto labelHint = Label::create("Try searching other keywords, or change some filters!", cw::font::chat);
    labelHint->setScale(0.625f);
    labelHint->setOpacity(200);
    labelHint->setAnchorPoint({ 0.5, 1 });
    labelHint->setAlignment(Label::Alignment::Center);
    labelHint->setLimitLabelWidth(getScaledContentWidth() * 0.875f, labelHint->getScale(), labelHint->getScale() * 0.125f);
    labelHint->setPosition(getScaledContentSize() / 2.0f);

    addChild(labelHint);

    return true;
}

MenuNothingNode* MenuNothingNode::create(CCSize const& size, CCPoint const& pos) {
    auto ret = new MenuNothingNode();
    if (ret->init(size, pos)) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
}

struct MenuOptionCell::Impl final {
    IncompatType incompat = IncompatType::None;  // If this option is usable

    SharedOption option;  // A view into the option value :)

    bool hasInternet = false;  // would rather call doWeHaveInternet once

    Toggler* toggler = nullptr;      // The toggler for the option
    CCNode* newContainer = nullptr;  // Container for the "New!" label and icon

    Callback pinCallback = nullptr;  // Callback for when the option is pinned or unpinned

    // Save the current state of the toggler as the option state
    void saveTogglerState() {
        auto saved = options::get(option->getID());
        if (toggler) options::set(option->getID(), toggler->isToggled(), saved.pin, saved.viewed);

        hideNewLabel();
    }

    // Notify the user if this option is not compatible for their current platform
    void notifyIncompats() const {
        switch (incompat) {
            default: return;

            case IncompatType::Platform: {
                log::warn("Option {} is unavailable for platform {}", option->getID(), GEODE_PLATFORM_SHORT_IDENTIFIER);
                Notification::create(fmt::format("{} is not available for {}", option->getName(), GEODE_PLATFORM_NAME), NotificationIcon::Error)->show();
            }; break;

            case IncompatType::Internet: {
                log::warn("Option {} requires a working internet connection to function", option->getID());
                Notification::create(fmt::format("{} needs internet to work properly", option->getName()), NotificationIcon::Warning)->show();
            }; break;

            case IncompatType::Anticheat: {
                log::warn("Option {} is unavailable due to anti-cheats", option->getID());
                Notification::create(fmt::format("{} is not available with anti-cheat", option->getName()), NotificationIcon::Error)->show();
            }; break;
        }
    }

    void hideNewLabel() {
        newContainer->setVisible(false);
    }

    constexpr auto getTierDescString(SillyTier silly, IncompatType incompat) const noexcept {
        if (incompat != IncompatType::None) return "<cp>OPTION UNAVAILABLE</c>";

        switch (silly) {
            default: [[fallthrough]];

            case SillyTier::None: return "Silly Tier - UNKNOWN";
            case SillyTier::Low: return "<cg>Silly Tier - LOW</c>";
            case SillyTier::Medium: return "<cy>Silly Tier - MEDIUM</c>";
            case SillyTier::High: return "<cr>Silly Tier - HIGH</c>";
        };
    }

    constexpr auto getTierColor(SillyTier silly) const noexcept {
        switch (silly) {
            default:
                return cw::colors::white;
                break;

            case SillyTier::Low:
                return cw::colors::green;
                break;

            case SillyTier::Medium:
                return cw::colors::yellow;
                break;

            case SillyTier::High:
                return cw::colors::red;
                break;
        };
    }
};

MenuOptionCell::MenuOptionCell() : m_impl(std::make_unique<Impl>()) {}
MenuOptionCell::~MenuOptionCell() = default;

bool MenuOptionCell::init(CCSize const& size, SharedOption option, ZStringView theme, bool devMode, bool hasInternet, bool view) {
    m_impl->option = std::move(option);
    m_impl->hasInternet = hasInternet;

    if (m_impl->option->isCheating() && OptionManager::get()->isAnticheatEnabled()) m_impl->incompat = IncompatType::Anticheat;

    if (m_impl->incompat == IncompatType::None) {
        // check for compatibility
        m_impl->incompat = IncompatType::Platform;
        for (auto const& p : m_impl->option->getSupportedPlatforms()) {
            if (p & GEODE_PLATFORM_TARGET) {
                m_impl->incompat = IncompatType::None;
                break;
            };
        };
    };

    if (!CCNode::init()) return false;

    setID(m_impl->option->getID());
    setContentSize(size);
    setAnchorPoint({ 0.5, 1 });

    auto bg = cue::createBackground(
        getContentSize(),
        {
            .opacity = 175,
            .sidePadding = 0.0f,
            .verticalPadding = 0.0f,
            .cornerRoundness = -0.125f,
            .texture = cw::sprites::squares::white,
        }
    );
    bg->setColor(m_impl->getTierColor(m_impl->option->getSillyTier()));

    addChildAtPosition(bg, Anchor::Center);

    // Horizontal layout: [toggle] [name] [info]
    auto x = 2.5f;

    if (!view) {
        // toggler for the option
        m_impl->toggler = Toggler::createWithStandardSprites(
            0.875f,
            [this](auto, bool now) {
                if (m_impl->incompat == IncompatType::None) {
                    auto saved = options::get(m_impl->option->getID());
                    options::set(m_impl->option->getID(), now, saved.pin, true);

                    if (m_impl->option->isRestartRequired()) {
                        Notification::create("Restart required!", NotificationIcon::Warning)->show();
                        log::warn("Restart required to apply option {}", m_impl->option->getID());
                    };

                    log::info("Option {} now set to {}", m_impl->option->getName(), now ? "enabled" : "disabled");

                    m_impl->hideNewLabel();
                } else {
                    m_impl->toggler->setToggled(false);
                };

                m_impl->notifyIncompats();
            }
        );
        m_impl->toggler->setID("toggler");
        m_impl->toggler->setAnchorPoint(cw::anchor::center);
        m_impl->toggler->setScaleMultiplier(1.25f);
        m_impl->toggler->setScale(0.875f);

        // Set toggler state based on saved mod option value
        m_impl->toggler->setToggled(options::isEnabled(m_impl->option->getID()));

        addChildAtPosition(m_impl->toggler, Anchor::Left, { x + (m_impl->toggler->getScaledContentWidth() / 2.0f), 0.0f });

        x += m_impl->toggler->getScaledContentWidth() + 2.5f;
    };

    auto labelWidth = getContentWidth() - 50.0f - x;

    // name of the joke
    auto nameLabel = Label::create(m_impl->option->getName(), cw::font::big);
    nameLabel->setID("name-label");
    nameLabel->setScale(0.4f);
    nameLabel->setLimitLabelWidth(labelWidth, 0.4f, 0.125f);
    nameLabel->setAnchorPoint({ 0.0f, 0.5f });

    auto categoryLabel = Label::create(m_impl->option->getCategory(), cw::font::gold);
    categoryLabel->setID("category-label");
    categoryLabel->setScale(0.25f);
    categoryLabel->setLimitLabelWidth(labelWidth, 0.25f, 0.125f);
    categoryLabel->setAnchorPoint({ 0.0f, 0.5f });
    categoryLabel->setOpacity(200);

    addChildAtPosition(nameLabel, Anchor::Left, { x, 0.0f });
    addChildAtPosition(categoryLabel, Anchor::Left, { x, 10.0f });

    auto menuLayout = RowLayout::create()
                          ->setGap(5.0f)
                          ->setAutoScale(false)
                          ->setAxisReverse(true)
                          ->setAutoGrowAxis(0.0f);

    auto menu = CCNode::create();
    menu->setID("menu");
    menu->setAnchorPoint({ 1.0f, 0.5f });
    menu->setContentSize({ 0.0f, 0.0f });
    menu->setLayout(menuLayout);

    addChildAtPosition(menu, Anchor::Right, { -7.5f, 0.0f });

    auto onlineCompat = (m_impl->option->isOnline() ? m_impl->hasInternet : true);
    if (m_impl->incompat == IncompatType::None && !onlineCompat) m_impl->incompat = IncompatType::Internet;

    // info button
    auto infoBtn = Button::createWithSpriteFrameName(
        (m_impl->incompat == IncompatType::None) ? (onlineCompat ? cw::sprites::info : "info-warning.png"_gode) : "info-alert.png"_gode,
        [this](auto) {
            auto formatDesc = fmt::format("{}\n\n{}{}{}", (m_impl->option->getDescription().size() > 0) ? m_impl->option->getDescription() : "<cc>No description provided.</c>", m_impl->option->isOnline() ? "<co>An internet connection is required.</c>\n" : "", m_impl->getTierDescString(m_impl->option->getSillyTier(), m_impl->incompat), m_impl->option->isCheating() ? "\n<ca>Cheat Option</c>" : "");

            cw::showAlert(
                m_impl->option->getName(),
                std::move(formatDesc),
                "OK",
                nullptr,
                nullptr,
                375.0f
            );

            auto saved = options::get(m_impl->option->getID());
            if (!saved.viewed) {
                options::set(m_impl->option->getID(), saved.enabled, saved.pin, true);
                m_impl->hideNewLabel();
            };

            m_impl->notifyIncompats();
        }
    );
    infoBtn->setID("info-btn");
    infoBtn->setScale(onlineCompat ? 0.75f : 0.625f);

    menu->addChild(infoBtn);

    if (!view) {
        auto pinOff = CCSprite::createWithSpriteFrameName("pin.png"_gode);
        pinOff->setOpacity(75);
        auto pinOn = CCSprite::createWithSpriteFrameName("pin.png"_gode);
        pinOn->setOpacity(225);

        pinOff->setBlendFunc({ GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA });
        pinOn->setColor(themes::getColor(theme));

        auto pinBtn = Toggler::create(pinOff, pinOn, [this](auto, bool toggled) {
            options::set(m_impl->option->getID(), options::isEnabled(m_impl->option->getID()), toggled, true);

            if (m_impl->pinCallback) m_impl->pinCallback();
            m_impl->hideNewLabel();
        });
        pinBtn->setID("pin-btn");
        pinBtn->setScale(0.425f);

        pinBtn->setToggled(options::isPinned(m_impl->option->getID()));

        menu->addChild(pinBtn);

        auto newContainerLayout = RowLayout::create()
                                      ->setGap(1.25f)
                                      ->setAutoScale(false)
                                      ->setAutoGrowAxis(0.0f);

        m_impl->newContainer = CCNode::create();
        m_impl->newContainer->setID("new-option-container");
        m_impl->newContainer->setAnchorPoint({ 0, 0.5 });
        m_impl->newContainer->setLayout(newContainerLayout);
        m_impl->newContainer->setZOrder(9);

        addChildAtPosition(m_impl->newContainer, Anchor::BottomLeft, { x, 5.25f });

        auto newIcon = CCSprite::createWithSpriteFrameName("updates-available.png"_gode);
        newIcon->setID("new-option-icon");
        newIcon->setScale(0.25f);

        auto newLabel = Label::create("New!", cw::font::big);
        newLabel->setID("new-option-label");
        newLabel->setScale(0.25f);
        newLabel->setColor(cw::colors::cyan);

        m_impl->newContainer->addChild(newIcon);
        m_impl->newContainer->addChild(newLabel);

        m_impl->newContainer->updateLayout();

        m_impl->newContainer->setVisible(!options::isViewed(m_impl->option->getID()));
        m_impl->newContainer->setScale(0.75f);

        if (m_impl->incompat != IncompatType::None) {
            m_impl->toggler->setToggled(false);

            static_cast<CCSprite*>(m_impl->toggler->getOffNode())->setColor(cw::colors::gray);
            static_cast<CCSprite*>(m_impl->toggler->getOnNode())->setColor(cw::colors::gray);

            bg->setColor(cw::colors::gray);

            nameLabel->setColor(cw::colors::gray);
            categoryLabel->setColor(cw::colors::gray);

            m_impl->saveTogglerState();
        };

        if (devMode) {
            auto idLabel = Label::create(fmt::format("{} ({}) | {} delegate(s)", m_impl->option->getID(), m_impl->option->getIDHash(), options::getDelegates(m_impl->option->getID())), cw::font::chat);
            idLabel->setID("id-label");
            idLabel->setScale(0.375f);
            idLabel->setAlignment(Label::Alignment::Right);
            idLabel->setLimitLabelWidth(getScaledContentWidth() - 65.0f);
            idLabel->setAnchorPoint({ 1, 0 });
            idLabel->setColor(cw::colors::black);
            idLabel->setOpacity(125);

            addChildAtPosition(idLabel, Anchor::BottomRight, { -2.5f, 2.5f });
        };
    };

    menu->updateLayout();

    return true;
}

void MenuOptionCell::setPinCallback(Callback&& callback) {
    m_impl->pinCallback = std::move(callback);
}

SharedOption const& MenuOptionCell::getOption() const noexcept {
    return m_impl->option;
}

bool MenuOptionCell::isCompatible() const noexcept {
    return m_impl->incompat != IncompatType::Platform;
}

MenuOptionCell* MenuOptionCell::createView(float width, SharedOption option, ZStringView theme) {
    return create({ width, 30.0f }, option, theme, false, true, true);
}

MenuOptionCell* MenuOptionCell::create(CCSize const& size, SharedOption option, ZStringView theme, bool devMode, bool hasInternet, bool view) {
    auto ret = new MenuOptionCell();
    if (ret->init(size, std::move(option), theme, devMode, hasInternet, view)) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
}