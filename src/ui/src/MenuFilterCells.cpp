#include "../MenuFilterCells.hpp"

#include <Utils.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

static constexpr auto filterId = "abcdefghijklmnopqrstuvwxyz0123456789-_./ ";

bool MenuCategoryFilterCell::init(CCSize const& size, std::string category) {
    m_category = std::move(category);

    if (!CCMenu::init()) return false;

    // lol
    setID(str::join(str::split(str::filter(str::toLower(m_category), filterId), " "), "-"));
    setContentSize(size);
    setAnchorPoint({0.5, 1});

    auto bg = cue::createBackground(
        getScaledContentSize(),
        {
            .cornerRoundness = -0.25f,
        });
    bg->setPosition(getScaledContentSize() / 2.f);

    addChild(bg, -1);

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
    m_toggler->setAnchorPoint({0.5f, 0.5f});
    m_toggler->setPosition({10.f, getScaledContentHeight() / 2.f});
    m_toggler->setScale(0.875f);

    addChild(m_toggler);

    // name of the joke
    auto nameLabel = CCLabelBMFont::create(
        m_category.c_str(),
        "goldFont.fnt",
        getScaledContentWidth() - 35.f,
        kCCTextAlignmentLeft);
    nameLabel->setID("name-label");
    nameLabel->setLineBreakWithoutSpace(true);
    nameLabel->setAnchorPoint({0.f, 0.5f});
    nameLabel->setPosition({20.f, getScaledContentHeight() / 2.f});
    nameLabel->setScale(0.375f);

    addChild(nameLabel);

    return true;
};

void MenuCategoryFilterCell::onToggle(CCObject* sender) {
    if (m_toggler) {
        if (m_toggleCallback) m_toggleCallback(m_category, !m_toggler->isOn());
    };
};

void MenuCategoryFilterCell::setToggleCallback(Callback&& callback) {
    m_toggleCallback = std::move(callback);
};

void MenuCategoryFilterCell::setToggled(bool on) {
    if (m_toggler) m_toggler->toggle(on);
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
            .opacity = 125,
            .texture = "geode.loader/white-square.png",
        });
    bg->setColor(color);

    auto tierLabel = CCLabelBMFont::create(label.c_str(), "bigFont.fnt");
    tierLabel->setID("label");
    tierLabel->setScale(0.5f);
    tierLabel->setAnchorPoint({0.5, 0.5});
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