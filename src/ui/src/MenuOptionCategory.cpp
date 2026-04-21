#include "../MenuOptionCategory.h"

#include <Utils.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class MenuOptionCategory::Impl final {
public:
    std::string category = "";  // The category name

    CCMenuItemToggler* toggler = nullptr;  // The toggler for the option

    Callback toggleCallback = nullptr;  // Callback for when the category is toggled
};

MenuOptionCategory::MenuOptionCategory() : m_impl(std::make_unique<Impl>()) {};
MenuOptionCategory::~MenuOptionCategory() {};

bool MenuOptionCategory::init(CCSize const& size, std::string category) {
    m_impl->category = std::move(category);

    if (!CCMenu::init()) return false;

    // lol
    setID(str::join(str::split(str::filter(str::toLower(m_impl->category), "abcdefghijklmnopqrstuvwxyz0123456789-_./ "), " "), "-"));
    setContentSize(size);
    setAnchorPoint({0.5, 1});

    auto bg = NineSlice::create(themes::square);
    bg->setID("background");
    bg->setAnchorPoint({0, 0});
    bg->setContentSize(getScaledContentSize());
    bg->setScaleMultiplier(0.2f);
    bg->setOpacity(40);

    addChild(bg, -1);

    auto togglerOff = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    togglerOff->setScale(0.5f);
    auto togglerOn = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    togglerOn->setScale(0.5f);

    // toggler for the category
    m_impl->toggler = CCMenuItemToggler::create(
        togglerOff,
        togglerOn,
        this,
        menu_selector(MenuOptionCategory::onToggle));
    m_impl->toggler->setID("toggler");
    m_impl->toggler->setAnchorPoint({0.5f, 0.5f});
    m_impl->toggler->setPosition({10.f, getScaledContentHeight() / 2.f});
    m_impl->toggler->setScale(0.875f);

    addChild(m_impl->toggler);

    // name of the joke
    auto nameLabel = CCLabelBMFont::create(
        m_impl->category.c_str(),
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

void MenuOptionCategory::onToggle(CCObject* sender) {
    if (m_impl->toggler) {
        if (m_impl->toggleCallback) m_impl->toggleCallback(m_impl->category, !m_impl->toggler->isOn());
    };
};

void MenuOptionCategory::setToggleCallback(Callback&& callback) & {
    m_impl->toggleCallback = std::move(callback);
};

void MenuOptionCategory::setToggled(bool on) & {
    if (m_impl->toggler) m_impl->toggler->toggle(on);
};

ZStringView MenuOptionCategory::getCategory() const noexcept {
    return m_impl->category;
};

MenuOptionCategory* MenuOptionCategory::create(CCSize const& size, std::string category) {
    auto ret = new MenuOptionCategory();
    if (ret->init(size, std::move(category))) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};