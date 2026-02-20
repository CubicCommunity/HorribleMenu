#include "../OptionCategoryItem.hpp"

#include <Utils.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class OptionCategoryItem::Impl final {
public:
    std::string m_category = ""; // The category name

    CCMenuItemToggler* m_toggler = nullptr; // The toggler for the option
};

OptionCategoryItem::OptionCategoryItem() {
    m_impl = std::make_unique<Impl>();
};

OptionCategoryItem::~OptionCategoryItem() {};

bool OptionCategoryItem::init(CCSize const& size, std::string category) {
    m_impl->m_category = std::move(category);

    if (!CCMenu::init()) return false;

    setID(str::join(str::split(str::filter(str::toLower(m_impl->m_category), "abcdefghijklmnopqrstuvwxyz0123456789-_./ "), " "), "-"));
    setContentSize(size);
    setAnchorPoint({ 0.5, 1 });

    auto bg = CCScale9Sprite::create("square02_001.png");
    bg->setID("background");
    bg->setScale(0.2f);
    bg->setAnchorPoint({ 0, 0 });
    bg->setContentSize(getScaledContentSize() * 5.f);
    bg->setOpacity(40);

    addChild(bg, -1);

    auto togglerOff = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    togglerOff->setScale(0.5f);
    auto togglerOn = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    togglerOn->setScale(0.5f);

    // toggler for the category
    m_impl->m_toggler = CCMenuItemToggler::create(
        togglerOff,
        togglerOn,
        this,
        menu_selector(OptionCategoryItem::onToggle)
    );
    m_impl->m_toggler->setID("toggler");
    m_impl->m_toggler->setAnchorPoint({ 0.5f, 0.5f });
    m_impl->m_toggler->setPosition({ 10.f, getScaledContentHeight() / 2.f });
    m_impl->m_toggler->setScale(0.875f);

    addChild(m_impl->m_toggler);

    // name of the joke
    auto nameLabel = CCLabelBMFont::create(
        m_impl->m_category.c_str(),
        "goldFont.fnt",
        getScaledContentWidth() - 35.f,
        kCCTextAlignmentLeft
    );
    nameLabel->setID("name-label");
    nameLabel->setLineBreakWithoutSpace(true);
    nameLabel->setAnchorPoint({ 0.f, 0.5f });
    nameLabel->setPosition({ 20.f, getScaledContentHeight() / 2.f });
    nameLabel->setScale(0.375f);

    addChild(nameLabel);

    return true;
};

ListenerResult OptionCategoryItem::OnCategory(std::string_view category, bool enabled) {
    if (m_impl->m_toggler) if (category != m_impl->m_category) m_impl->m_toggler->toggle(false);
    return ListenerResult::Propagate;
};

void OptionCategoryItem::onToggle(CCObject* sender) {
    if (m_impl->m_toggler) CategoryEvent().send(m_impl->m_category, !m_impl->m_toggler->isOn());
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