#include "../InputMeter.hpp"

#include <Util.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define PACE_RANGES 250.f, 175.f

struct InputMeter::Impl final {
    float chance = 0.f;
    float pace = rng::get(PACE_RANGES);

    FMODLevelVisualizer* meter = nullptr;
    CCSprite* indicator = nullptr;

    constexpr ccColor3B const& getChanceColor(float chance) {
        if (chance >= 85.f) return colors::red;
        if (chance >= 60.f) return colors::yellow;

        return colors::green;
    };
};

InputMeter::InputMeter() : m_impl(std::make_unique<Impl>()) {};
InputMeter::~InputMeter() {};

bool InputMeter::init() {
    if (!CCNode::init()) return false;

    setAnchorPoint(anchor::center);
    setContentSize({10.f, 100.f});

    m_impl->meter = FMODLevelVisualizer::create();
    m_impl->meter->setID("meter");
    m_impl->meter->updateVisualizer(m_impl->chance, m_impl->chance, 1.f);

    addChildAtPosition(m_impl->meter, Anchor::Center, {}, false);

    m_impl->indicator = CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
    m_impl->indicator->setID("indicator");
    m_impl->indicator->setScale(0.875f);
    m_impl->indicator->setPosition({getScaledContentWidth() * -1.f, m_impl->chance});
    m_impl->indicator->setColor(m_impl->getChanceColor(m_impl->chance));

    addChild(m_impl->indicator, 1);

    scheduleUpdate();

    return true;
};

void InputMeter::update(float dt) {
    if (m_impl->meter && m_impl->indicator) {
        m_impl->chance += dt * m_impl->pace;

        m_impl->meter->updateVisualizer(m_impl->chance / 100.f, m_impl->chance / 100.f, dt);

        m_impl->indicator->setPositionY(m_impl->chance);
        m_impl->indicator->setColor(m_impl->getChanceColor(m_impl->chance));

        if (m_impl->chance <= 0.f) {
            m_impl->chance = 0.f;
            m_impl->pace = rng::get(PACE_RANGES);
        } else if (m_impl->chance >= 100.f) {
            m_impl->chance = 100.f;
            m_impl->pace = rng::get(PACE_RANGES) * -1.f;
        };
    };
};

void InputMeter::playPause(bool play) {
    play ? scheduleUpdate() : unscheduleUpdate();
};

bool InputMeter::isDeath() const noexcept {
    return m_impl->chance >= 85.f;
};

bool InputMeter::isDanger() const noexcept {
    return m_impl->chance <= 85.f && m_impl->chance >= 60.f;
};

InputMeter* InputMeter::create() {
    auto ret = new InputMeter();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};