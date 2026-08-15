#include "../MenuFilterCells.hpp"

#include <Util.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

static constexpr auto filterId = "abcdefghijklmnopqrstuvwxyz0123456789-_./ ";

bool MenuCategoryFilterCell::init(CCSize const& size, std::string category) {
    m_category = std::move(category);

    if (!CCMenu::init()) return false;

    // lol
    setID(str::join(asp::iter::split(str::filter(str::toLower(m_category), filterId), " ").collect(), "-"));
    setContentSize(size);
    setAnchorPoint({0.5, 1});

    m_bg = cue::attachBackground(
        this,
        {
            .opacity = 175,
            .sidePadding = 0.f,
            .verticalPadding = 0.f,
            .cornerRoundness = -0.25f,
            .texture = themes::square,
        });
    m_bg->setColor(colors::gray);

    auto togglerOff = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    togglerOff->setScale(0.5f);
    auto togglerOn = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    togglerOn->setScale(0.5f);

    // toggler for the category
    m_toggler = CCMenuItemToggler::create(
        togglerOff,
        togglerOn,
        this,
        menu_selector(MenuCategoryFilterCell::onToggle));
    m_toggler->setID("toggler");
    m_toggler->setAnchorPoint(anchor::center);
    m_toggler->setPosition({10.f, getScaledContentHeight() / 2.f});
    m_toggler->setScale(0.875f);

    addChild(m_toggler);

    // name of the joke
    auto nameLabel = Label::create(m_category.c_str(), font::gold);
    nameLabel->setID("name-label");
    nameLabel->setScale(0.375f);
    nameLabel->setLimitLabelWidth(getScaledContentWidth() - 35.f, 0.375f, 0.125f);
    nameLabel->setPosition({20.f, getScaledContentHeight() / 2.f});
    nameLabel->setAnchorPoint({0.f, 0.5f});

    addChild(nameLabel);

    return true;
};

void MenuCategoryFilterCell::onToggle(CCObject* sender) {
    if (auto toggler = typeinfo_cast<CCMenuItemToggler*>(sender)) {
        auto on = !toggler->isOn();

        if (m_toggleCallback) m_toggleCallback(m_category, on);
        if (m_bg) m_bg->setColor(on ? colors::gold : colors::gray);
    };
};

void MenuCategoryFilterCell::setToggleCallback(Callback&& callback) {
    m_toggleCallback = std::move(callback);
};

void MenuCategoryFilterCell::setToggled(bool on) {
    if (m_toggler) m_toggler->toggle(on);
    if (m_bg) m_bg->setColor(on ? colors::gold : colors::gray);
};

ZStringView MenuCategoryFilterCell::getCategory() const noexcept {
    return m_category;
};

MenuCategoryFilterCell* MenuCategoryFilterCell::create(CCSize const& size, std::string category) {
    auto ret = new MenuCategoryFilterCell();
    if (ret->init(size, std::move(category))) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};

bool MenuSillyFilterCell::init(CCSize const& size, SillyTier silly, std::string id, ZStringView label, ccColor3B const& color) {
    m_silly = silly;

    if (!CCNode::init()) return false;

    setID(std::move(id));
    setContentSize(size);
    setAnchorPoint({0, 0.5});
    setZOrder(-1);

    auto bg = cue::attachBackground(
        this,
        {
            .opacity = 175,
            .sidePadding = 0.f,
            .verticalPadding = 0.f,
            .texture = themes::square,
        });
    bg->setColor(color);

    auto tierLabel = Label::create(label.c_str(), font::big);
    tierLabel->setID("label");
    tierLabel->setScale(0.5f);
    tierLabel->setAnchorPoint(anchor::center);
    tierLabel->setPosition(getScaledContentSize() / 2.f);

    addChild(tierLabel, 1);

    return true;
};

SillyTier MenuSillyFilterCell::getSillyTier() const noexcept {
    return m_silly;
};

MenuSillyFilterCell* MenuSillyFilterCell::create(CCSize const& size, SillyTier silly, std::string id, ZStringView label, ccColor3B const& color) {
    auto ret = new MenuSillyFilterCell();
    if (ret->init(size, silly, std::move(id), label, color)) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};