#include "../CategoryItem.hpp"

#include <Utils.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

CategoryEvent::CategoryEvent(std::string id, bool enabled) : m_id(std::move(id)), m_enabled(enabled) {};

std::string const& CategoryEvent::getId() const {
    return m_id;
};

bool CategoryEvent::isEnabled() const {
    return m_enabled;
};

ListenerResult CategoryEventFilter::handle(std::function<Callback> fn, CategoryEvent* event) {
    return fn(event);
};

class CategoryItem::Impl final {
public:
    std::string m_category = ""; // The category name

    CCMenuItemToggler* m_toggler = nullptr; // The toggler for the option
};

CategoryItem::CategoryItem() {
    m_impl = std::make_unique<Impl>();
};

CategoryItem::~CategoryItem() {};

bool CategoryItem::init(CCSize const& size, std::string_view category) {
    m_impl->m_category = category;

    if (!CCMenu::init()) return false;

    setID(str::join(str::split(str::filter(str::toLower(category.data()), "abcdefghijklmnopqrstuvwxyz0123456789 "), " "), "-"));
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
        menu_selector(CategoryItem::onToggle)
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

ListenerResult CategoryItem::OnCategory(std::string_view category, bool enabled) {
    if (m_impl->m_toggler) {
        if (category != m_impl->m_category) m_impl->m_toggler->toggle(false);
    };

    return ListenerResult::Propagate;
};

void CategoryItem::onToggle(CCObject* sender) {
    if (m_impl->m_toggler) CategoryEvent(m_impl->m_category, !m_impl->m_toggler->isOn()).post();
};

CategoryItem* CategoryItem::create(CCSize const& size, std::string_view category) {
    auto ret = new CategoryItem();
    if (ret->init(size, category)) {
        ret->autorelease();
        return ret;
    };

    CC_SAFE_DELETE(ret);
    return nullptr;
};