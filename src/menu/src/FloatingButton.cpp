#include "../FloatingButton.hpp"

#include <Utils.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class FloatingButton::Impl final {
public:
    bool m_inLevel = horribleMod->getSettingValue<bool>("floating-button-level");

    float m_scale = static_cast<float>(horribleMod->getSettingValue<double>("floating-button-scale"));
    int64_t m_opacity = horribleMod->getSettingValue<int64_t>("floating-button-opacity");

    bool m_isDragging = false;
    bool m_isMoving = false;

    CCSize const m_screenSize = CCDirector::sharedDirector()->getWinSize();
    CCPoint m_dragStartPos = { 0, 0 };

    Ref<CircleButtonSprite> m_sprite = nullptr;

    bool m_isAnimating = false;
};

FloatingButton::FloatingButton() {
    m_impl = std::make_unique<Impl>();
};

FloatingButton::~FloatingButton() {};

bool FloatingButton::init() {
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

    m_impl->m_sprite = CircleButtonSprite::createWithSprite(
        "icon.png"_spr,
        0.925f
    );
    m_impl->m_sprite->setAnchorPoint({ 0.5, 0.5 });

    setContentSize(m_impl->m_sprite->getScaledContentSize());

    m_impl->m_sprite->setPosition(getScaledContentSize() / 2.f);

    setScale(m_impl->m_scale); // set initial scale
    setOpacity(m_impl->m_opacity); // set initial opacity

    setVisible(horribleMod->getSettingValue<bool>("floating-button")); // set initial visibility

    addChild(m_impl->m_sprite);

    return true;
};

void FloatingButton::setOpacity(GLubyte opacity) {
    m_impl->m_opacity = opacity;
    if (m_impl->m_sprite) m_impl->m_sprite->setOpacity(isVisible() ? opacity : 0);
};

void FloatingButton::setShowInLevel(bool show) {
    m_impl->m_inLevel = show;
};

void FloatingButton::setScale(float scale) {
    m_impl->m_scale = scale;

    if (!m_impl->m_isDragging && !m_impl->m_isAnimating) {
        if (m_impl->m_sprite) {
            m_impl->m_sprite->setScale(scale);
            setContentSize(m_impl->m_sprite->getScaledContentSize());
        };
    };
};

void FloatingButton::setPosition(CCPoint const& position) {
    if (m_impl->m_sprite) {
        auto halfX = m_impl->m_sprite->getScaledContentWidth() / 2.f;
        auto halfY = m_impl->m_sprite->getScaledContentHeight() / 2.f;

        auto clampX = std::clamp<float>(position.x, halfX, m_impl->m_screenSize.width - halfX);
        auto clampY = std::clamp<float>(position.y, halfY, m_impl->m_screenSize.height - halfY);

        auto clampPos = ccp(clampX, clampY);
        CCLayer::setPosition(clampPos);

        // Save only when not dragging
        if (!m_impl->m_isDragging) {
            horribleMod->setSavedValue<float>("button-x", clampPos.x);
            horribleMod->setSavedValue<float>("button-y", clampPos.y);
        };
    } else {
        CCLayer::setPosition(position);
    };
};

bool FloatingButton::ccTouchBegan(CCTouch* touch, CCEvent* ev) {
    if (m_impl->m_sprite && isVisible()) {
        CCPoint const touchLocation = convertToNodeSpace(touch->getLocation());

        auto box = m_impl->m_sprite->boundingBox();
        if (box.containsPoint(touchLocation)) {
            m_impl->m_isDragging = true;

            m_impl->m_dragStartPos = ccpSub(getPosition(), touch->getLocation());

            m_impl->m_sprite->stopAllActions();
            m_impl->m_isAnimating = true;
            m_impl->m_sprite->runAction(CCSequence::createWithTwoActions(
                CCSpawn::createWithTwoActions(
                    CCEaseExponentialOut::create(CCScaleTo::create(0.25f, m_impl->m_scale * 0.875f)),
                    CCFadeTo::create(0.25f, 255)
                ),
                CCCallFunc::create(this, callfunc_selector(FloatingButton::onScaleEnd))
            ));

            return true; // swallow touch
        };
    };

    return false;
};

void FloatingButton::ccTouchMoved(CCTouch* touch, CCEvent* ev) {
    if (m_impl->m_isDragging) {
        CCPoint const touchLocation = touch->getLocation();
        CCPoint const newLocation = ccpAdd(touchLocation, m_impl->m_dragStartPos);

        setPosition(newLocation);

        m_impl->m_isMoving = true;
    };
};

void FloatingButton::ccTouchEnded(CCTouch* touch, CCEvent* ev) {
    if (!m_impl->m_isMoving) menu::open();

    // reset state
    m_impl->m_isDragging = false;
    m_impl->m_isMoving = false;

    // store position
    horribleMod->setSavedValue<float>("button-x", getPosition().x);
    horribleMod->setSavedValue<float>("button-y", getPosition().y);

    m_impl->m_isAnimating = true;

    if (m_impl->m_sprite) {
        // reset scale
        m_impl->m_sprite->stopAllActions();
        m_impl->m_sprite->runAction(CCSequence::create(
            CCSpawn::createWithTwoActions(
                CCFadeTo::create(0.125f, 255),
                CCEaseElasticOut::create(CCScaleTo::create(0.875f, m_impl->m_scale))
            ),
            CCCallFunc::create(this, callfunc_selector(FloatingButton::onScaleEnd)),
            CCDelayTime::create(1.f),
            CCFadeTo::create(0.5f, m_impl->m_opacity),
            nullptr));
    };
};

void FloatingButton::onEnter() {
    CCLayer::onEnter();
    setTouchEnabled(true);
};

void FloatingButton::onScaleEnd() {
    m_impl->m_isAnimating = false;
};

int64_t FloatingButton::getOpacitySetting() const {
    return m_impl->m_opacity;
};

float FloatingButton::getScaleSetting() const {
    return m_impl->m_scale;
};

bool FloatingButton::showInLevel() const {
    return m_impl->m_inLevel;
};

FloatingButton* FloatingButton::create() {
    auto ret = new FloatingButton();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    };

    CC_SAFE_DELETE(ret);
    return nullptr;
};

FloatingButton* FloatingButton::get() {
    static auto inst = FloatingButton::create();
    return inst;
};