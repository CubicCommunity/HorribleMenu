#include "../OptionCategoryItem.h"

#include <Geode/Geode.hpp>
#include <Utils.h>

using namespace geode::prelude;
using namespace horrible::prelude;

class OptionCategoryItem::Impl final {
public:
    std::string category = "";  // The category name

    CCMenuItemToggler* toggler = nullptr;  // The toggler for the option
};

OptionCategoryItem::OptionCategoryItem() : m_impl(std::make_unique<Impl>()) {};
OptionCategoryItem::~OptionCategoryItem() {};

bool OptionCategoryItem::init(CCSize const& size, std::string category) {
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
        menu_selector(OptionCategoryItem::onToggle));
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

    addEventListener(
        CategoryEvent(),
        [this](std::string_view category, bool enabled) {
            if (m_impl->toggler) {
                if (category != m_impl->category) m_impl->toggler->toggle(false);
            };
        });

    return true;
};

void OptionCategoryItem::onToggle(CCObject* sender) {
    if (m_impl->toggler) CategoryEvent().send(m_impl->category, !m_impl->toggler->isOn());
};

OptionCategoryItem* OptionCategoryItem::create(CCSize const& size, std::string category) {
    auto ret = new OptionCategoryItem();
    if (ret->init(size, std::move(category))) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};