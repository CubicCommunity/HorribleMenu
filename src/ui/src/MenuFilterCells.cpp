#include "../MenuFilterCells.hpp"

#include <Util.h>

#include <rhatt/prelude.h>

#include <asp/iter.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace cs::brkd::horrible::prelude;

static constexpr auto filterId = "abcdefghijklmnopqrstuvwxyz0123456789-_./ ";

bool MenuCategoryFilterCell::init(CCSize const& size, std::string category) {
    m_category = std::move(category);

    if (!CCMenu::init()) return false;

    setContentSize(size);
    setAnchorPoint({ 0.5, 1 });

    m_bg = cue::attachBackground(
        this,
        {
            .opacity = 175,
            .sidePadding = 0.0f,
            .verticalPadding = 0.0f,
            .cornerRoundness = -0.25f,
            .texture = cw::sprites::squares::white,
        }
    );
    m_bg->setColor(cw::colors::gray);

    // toggler for the category
    m_toggler = Toggler::createWithStandardSprites(
        0.5f,
        [this](auto, bool on) {
            if (m_toggleCallback) m_toggleCallback(m_category, on);
            if (m_bg) m_bg->setColor(on ? cw::colors::gold : cw::colors::gray);
        }
    );
    m_toggler->setID("toggler");
    m_toggler->setAnchorPoint(cw::anchor::center);
    m_toggler->setPosition({ 10.0f, getScaledContentHeight() / 2.0f });

    addChild(m_toggler);

    // name of the joke
    auto nameLabel = Label::create(m_category, cw::font::gold);
    nameLabel->setID("name-label");
    nameLabel->setScale(0.375f);
    nameLabel->setLimitLabelWidth(getScaledContentWidth() - 35.0f, 0.375f, 0.125f);
    nameLabel->setPosition({ 20.0f, getScaledContentHeight() / 2.0f });
    nameLabel->setAnchorPoint({ 0.0f, 0.5f });

    addChild(nameLabel);

    return true;
}

void MenuCategoryFilterCell::setToggleCallback(Callback&& callback) {
    m_toggleCallback = std::move(callback);
}

void MenuCategoryFilterCell::setToggled(bool on) {
    if (m_toggler) m_toggler->setToggled(on);
    if (m_bg) m_bg->setColor(on ? cw::colors::gold : cw::colors::gray);
}

ZStringView MenuCategoryFilterCell::getCategory() const noexcept {
    return m_category;
}

MenuCategoryFilterCell* MenuCategoryFilterCell::create(CCSize const& size, std::string category) {
    auto ret = new MenuCategoryFilterCell();
    if (ret->init(size, std::move(category))) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
}

bool MenuSillyFilterCell::init(CCSize const& size, SillyTier silly, std::string id, std::string label, ccColor3B const& color) {
    m_silly = silly;

    if (!CCNode::init()) return false;

    setID(std::move(id));
    setContentSize(size);
    setAnchorPoint({ 0, 0.5 });
    setZOrder(-1);

    auto bg = cue::attachBackground(
        this,
        {
            .opacity = 175,
            .sidePadding = 0.0f,
            .verticalPadding = 0.0f,
            .texture = cw::sprites::squares::white,
        }
    );
    bg->setColor(color);

    auto tierLabel = Label::create(std::move(label), cw::font::big);
    tierLabel->setID("label");
    tierLabel->setScale(0.5f);
    tierLabel->setAnchorPoint(cw::anchor::center);
    tierLabel->setPosition(getScaledContentSize() / 2.0f);

    addChild(tierLabel, 1);

    return true;
}

SillyTier MenuSillyFilterCell::getSillyTier() const noexcept {
    return m_silly;
}

MenuSillyFilterCell* MenuSillyFilterCell::create(CCSize const& size, SillyTier silly, std::string id, std::string label, ccColor3B const& color) {
    auto ret = new MenuSillyFilterCell();
    if (ret->init(size, silly, std::move(id), std::move(label), color)) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
}