#include "../OptionMenuButton.h"

#include <Utils.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

Result<CCPoint> matjson::Serialize<CCPoint>::fromJson(matjson::Value const& value) {
    if (!value.isObject()) return Err("Expected an object");

    GEODE_UNWRAP_INTO(float x, value["x"].asDouble());
    GEODE_UNWRAP_INTO(float y, value["y"].asDouble());

    return Ok(CCPoint{x, y});
};

matjson::Value matjson::Serialize<CCPoint>::toJson(CCPoint const& value) {
    auto obj = matjson::Value();
    obj["x"] = value.x;
    obj["y"] = value.y;

    return obj;
};

class OptionMenuButton::Impl final {
public:
    bool inLevel = thisMod->getSettingValue<bool>("floating-btn-level");

    float scale = thisMod->getSettingValue<float>("floating-btn-scale");
    int64_t opacity = thisMod->getSettingValue<int64_t>("floating-btn-opacity");

    bool isDragging = false;
    bool isMoving = false;

    CCSize const screenSize = CCDirector::sharedDirector()->getWinSize();
    CCPoint dragStartPos = {0, 0};
    CCPoint comparePos = {0, 0};

    Ref<CircleButtonSprite> sprite = nullptr;

    bool isAnimating = false;

    std::string theme = thisMod->getSettingValue<std::string>("theme");
    std::string btnIcon = thisMod->getSettingValue<std::string>("floating-btn-icon");

    bool isDistant(CCPoint const& ccp1, CCPoint const& ccp2, float max) const {
        return ccpDistance(ccp1, ccp2) <= max;
    };
};

OptionMenuButton::OptionMenuButton() : m_impl(std::make_unique<Impl>()) {};
OptionMenuButton::~OptionMenuButton() {};

void OptionMenuButton::setupSprite() {
    if (auto sprite = m_impl->sprite.take()) sprite->removeMeAndCleanup();

    m_impl->sprite = CircleButtonSprite::createWithSpriteFrameName(
        themes::getIconSprite(m_impl->btnIcon),
        0.925f,
        themes::getCircleBaseColor(m_impl->theme));

    setContentSize(m_impl->sprite->getScaledContentSize());

    m_impl->sprite->setPosition(getScaledContentSize() / 2.f);

    setScale(m_impl->scale);      // set initial scale
    setOpacity(m_impl->opacity);  // set initial opacity

    setVisible(thisMod->getSettingValue<bool>(setting::FloatingBtn));  // set initial visibility

    addChild(m_impl->sprite);
};

bool OptionMenuButton::init() {
    if (!CCLayer::init()) return false;

    setID("menu-btn"_spr);
    setAnchorPoint({0.5, 0.5});
    setPosition(thisMod->getSavedValue<CCPoint>("menu-pos", m_impl->screenSize - 75.f));
    setTouchMode(kCCTouchesOneByOne);
    setTouchEnabled(true);
    setTouchPriority(-512);  // ewww touch priority
    setZOrder(99);

    setupSprite();

    return true;
};

void OptionMenuButton::setOpacity(GLubyte opacity) {
    m_impl->opacity = opacity;
    if (m_impl->sprite) m_impl->sprite->setOpacity(isVisible() ? opacity : 0);
};

void OptionMenuButton::setShowInLevel(bool show) {
    m_impl->inLevel = show;
};

void OptionMenuButton::setScale(float scale) {
    m_impl->scale = scale;

    if (!m_impl->isDragging && !m_impl->isAnimating) {
        if (m_impl->sprite) {
            m_impl->sprite->setScale(scale);
            setContentSize(m_impl->sprite->getScaledContentSize());
        };
    };
};

void OptionMenuButton::setTheme(std::string theme) {
    m_impl->theme = std::move(theme);
    setupSprite();
};

void OptionMenuButton::setButtonIcon(std::string icon) {
    m_impl->btnIcon = std::move(icon);
    setupSprite();
};

bool OptionMenuButton::ccTouchBegan(CCTouch* touch, CCEvent* ev) {
    if (!isVisible()) return false;

    if (m_impl->sprite) {
        auto const box = m_impl->sprite->boundingBox();

        if (box.containsPoint(convertToNodeSpace(touch->getLocation()))) {
            m_impl->isDragging = true;

            m_impl->comparePos = getPosition();
            m_impl->dragStartPos = ccpSub(getPosition(), touch->getLocation());

            log::debug("Menu position starts at ({}, {})", m_impl->dragStartPos.x, m_impl->dragStartPos.y);

            m_impl->sprite->stopAllActions();
            m_impl->isAnimating = true;
            m_impl->sprite->runAction(CCSequence::createWithTwoActions(
                CCSpawn::createWithTwoActions(
                    CCEaseExponentialOut::create(CCScaleTo::create(0.25f, m_impl->scale * 0.875f)),
                    CCFadeTo::create(0.25f, 255)),
                CCCallFunc::create(this, callfunc_selector(OptionMenuButton::onScaleEnd))));

            return true;  // swallow touch like a...
        };
    };

    return false;
};

void OptionMenuButton::ccTouchMoved(CCTouch* touch, CCEvent* ev) {
    if (m_impl->isDragging) {
        auto const touchLocation = touch->getLocation();
        auto const newLocation = ccpAdd(touchLocation, m_impl->dragStartPos);

        auto clampX = std::max(0.f, std::min(newLocation.x, m_impl->screenSize.width - getScaledContentWidth()));
        auto clampY = std::max(0.f, std::min(newLocation.y, m_impl->screenSize.height - getScaledContentHeight()));

        setPosition(ccp(clampX, clampY));
    };
};

void OptionMenuButton::ccTouchEnded(CCTouch* touch, CCEvent* ev) {
    if (m_impl->isDragging) {
        auto pos = ccpSub(getPosition(), touch->getLocation());
        if (m_impl->isDistant(m_impl->comparePos, getPosition(), 5.f)) menu::open();

        m_impl->isDragging = false;

        thisMod->setSavedValue<CCPoint>("menu-pos", getPosition());

        if (m_impl->sprite) {
            m_impl->isAnimating = true;

            // reset scale
            m_impl->sprite->stopAllActions();
            m_impl->sprite->runAction(CCSequence::create(
                CCSpawn::createWithTwoActions(
                    CCFadeTo::create(0.125f, 255),
                    CCEaseElasticOut::create(CCScaleTo::create(0.875f, m_impl->scale))),
                CCCallFunc::create(this, callfunc_selector(OptionMenuButton::onScaleEnd)),
                CCDelayTime::create(1.f),
                CCFadeTo::create(0.5f, m_impl->opacity / 1.25),
                nullptr));
        };

        m_impl->dragStartPos = pos;

        log::debug("Menu position stopped and saved at ({}, {})", m_impl->dragStartPos.x, m_impl->dragStartPos.y);
    };
};

void OptionMenuButton::onEnter() {
    CCLayer::onEnter();
    setTouchEnabled(true);
};

void OptionMenuButton::onScaleEnd() {
    m_impl->isAnimating = false;
};

int64_t OptionMenuButton::getOpacitySetting() const noexcept {
    return m_impl->opacity;
};

float OptionMenuButton::getScaleSetting() const noexcept {
    return m_impl->scale;
};

bool OptionMenuButton::showInLevel() const noexcept {
    return m_impl->inLevel;
};

OptionMenuButton* OptionMenuButton::create() {
    auto ret = new OptionMenuButton();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};

OptionMenuButton* OptionMenuButton::get() noexcept {
    static auto inst = OptionMenuButton::create();
    return inst;
};