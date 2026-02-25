#include "../OptionMenuButton.hpp"

#include <Utils.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class OptionMenuButton::Impl final {
public:
    bool inLevel = horribleMod->getSettingValue<bool>("floating-button-level");

    float scale = static_cast<float>(horribleMod->getSettingValue<double>("floating-button-scale"));
    int64_t opacity = horribleMod->getSettingValue<int64_t>("floating-button-opacity");

    bool isDragging = false;
    bool isMoving = false;

    CCSize const screenSize = CCDirector::sharedDirector()->getWinSize();
    CCPoint dragStartPos = { 0, 0 };

    Ref<CircleButtonSprite> sprite = nullptr;

    bool isAnimating = false;

    std::string theme = horribleMod->getSettingValue<std::string>("theme");
};

OptionMenuButton::OptionMenuButton() : m_impl(std::make_unique<Impl>()) {};
OptionMenuButton::~OptionMenuButton() {};

void OptionMenuButton::setupSprite() {
    m_impl->sprite = CircleButtonSprite::createWithSprite(
        "icon.png"_spr,
        0.925f,
        theme::getCircleBaseColor(m_impl->theme)
    );
    m_impl->sprite->setAnchorPoint({ 0.5, 0.5 });

    setContentSize(m_impl->sprite->getScaledContentSize());

    m_impl->sprite->setPosition(getScaledContentSize() / 2.f);

    setScale(m_impl->scale); // set initial scale
    setOpacity(m_impl->opacity); // set initial opacity

    setVisible(horribleMod->getSettingValue<bool>("floating-button")); // set initial visibility

    addChild(m_impl->sprite);
};

bool OptionMenuButton::init() {
    if (!CCLayer::init()) return false;

    // get the saved position
    float x = horribleMod->getSavedValue<float>("button-x", 100.f);
    float y = horribleMod->getSavedValue<float>("button-y", 125.f);

    setID("floating-button"_spr);
    setPosition({ x, y });
    setAnchorPoint({ 0.5, 0.5 });
    setTouchMode(kCCTouchesOneByOne);
    setTouchEnabled(true);
    setTouchPriority(-512);  // ewww touch priority
    setZOrder(9999);

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

void OptionMenuButton::setPosition(CCPoint const& position) {
    if (m_impl->sprite) {
        auto halfX = m_impl->sprite->getScaledContentWidth() / 2.f;
        auto halfY = m_impl->sprite->getScaledContentHeight() / 2.f;

        auto clampX = std::clamp<float>(position.x, halfX, m_impl->screenSize.width - halfX);
        auto clampY = std::clamp<float>(position.y, halfY, m_impl->screenSize.height - halfY);

        auto clampPos = ccp(clampX, clampY);
        CCLayer::setPosition(clampPos);

        // Save only when not dragging
        if (!m_impl->isDragging) {
            horribleMod->setSavedValue<float>("button-x", clampPos.x);
            horribleMod->setSavedValue<float>("button-y", clampPos.y);
        };
    } else {
        CCLayer::setPosition(position);
    };
};

void OptionMenuButton::setTheme(std::string theme) {
    m_impl->theme = std::move(theme);
    if (m_impl->sprite) m_impl->sprite->removeMeAndCleanup();
    setupSprite();
};

bool OptionMenuButton::ccTouchBegan(CCTouch* touch, CCEvent* ev) {
    if (m_impl->sprite && isVisible()) {
        CCPoint const touchLocation = convertToNodeSpace(touch->getLocation());

        auto box = m_impl->sprite->boundingBox();
        if (box.containsPoint(touchLocation)) {
            m_impl->isDragging = true;

            m_impl->dragStartPos = ccpSub(getPosition(), touch->getLocation());

            m_impl->sprite->stopAllActions();
            m_impl->isAnimating = true;
            m_impl->sprite->runAction(CCSequence::createWithTwoActions(
                CCSpawn::createWithTwoActions(
                    CCEaseExponentialOut::create(CCScaleTo::create(0.25f, m_impl->scale * 0.875f)),
                    CCFadeTo::create(0.25f, 255)
                ),
                CCCallFunc::create(this, callfunc_selector(OptionMenuButton::onScaleEnd))
            ));

            return true; // swallow touch
        };
    };

    return false;
};

void OptionMenuButton::ccTouchMoved(CCTouch* touch, CCEvent* ev) {
    if (m_impl->isDragging) {
        CCPoint const touchLocation = touch->getLocation();
        CCPoint const newLocation = ccpAdd(touchLocation, m_impl->dragStartPos);

        setPosition(newLocation);

        m_impl->isMoving = true;
    };
};

void OptionMenuButton::ccTouchEnded(CCTouch* touch, CCEvent* ev) {
    if (!m_impl->isMoving) menu::open();

    // reset state
    m_impl->isDragging = false;
    m_impl->isMoving = false;

    // store position
    horribleMod->setSavedValue<float>("button-x", getPosition().x);
    horribleMod->setSavedValue<float>("button-y", getPosition().y);

    m_impl->isAnimating = true;

    if (m_impl->sprite) {
        // reset scale
        m_impl->sprite->stopAllActions();
        m_impl->sprite->runAction(CCSequence::create(
            CCSpawn::createWithTwoActions(
                CCFadeTo::create(0.125f, 255),
                CCEaseElasticOut::create(CCScaleTo::create(0.875f, m_impl->scale))
            ),
            CCCallFunc::create(this, callfunc_selector(OptionMenuButton::onScaleEnd)),
            CCDelayTime::create(1.f),
            CCFadeTo::create(0.5f, m_impl->opacity),
            nullptr));
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