#include "../JumpHealthMeter.hpp"

#include <Util.h>

#include <rhatt/prelude.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace cs::brkd::horrible::prelude;

#define PACE_RANGES 50.0f, 25.0f

struct JumpHealthMeter::Impl final {
    uint8_t jumps = cw::rng::get(15, 7);
    float pace = cw::rng::get(PACE_RANGES);

    Label* healthLabel = nullptr;
    ProgressBar* healthMeter = nullptr;

    Callback callback = nullptr;

    constexpr auto getColorForJumps(uint8_t remaining) {
        if (jumps <= 6) return "<cr>";
        if (jumps <= 14) return "<cy>";

        return "<cg>";
    }

    void updateLabel() {
        if (healthLabel) healthLabel->setRichText(fmt::format("{}{}</c> Inputs Remaining", getColorForJumps(jumps), jumps));
    }
};

JumpHealthMeter::JumpHealthMeter() : m_impl(std::make_unique<Impl>()) {}
JumpHealthMeter::~JumpHealthMeter() = default;

bool JumpHealthMeter::init(Callback&& cb) {
    m_impl->callback = std::move(cb);

    if (!CCNode::init()) return false;

    setAnchorPoint(cw::anchor::center);
    setContentSize({ 150.0f, 27.5f });

    m_impl->healthMeter = ProgressBar::create();
    m_impl->healthMeter->setID("meter");
    m_impl->healthMeter->setFillColor(cw::colors::pink);
    m_impl->healthMeter->setAnchorPoint(cw::anchor::center);
    m_impl->healthMeter->updateProgress(0.0f);

    cue::rescaleToMatch(m_impl->healthMeter, getScaledContentWidth() - 8.75f);

    addChildAtPosition(m_impl->healthMeter, Anchor::Bottom, { 0.0f, 3.75f });

    m_impl->healthLabel = Label::createRich(fmt::format("{}{}</c> Inputs Remaining", m_impl->getColorForJumps(m_impl->jumps), m_impl->jumps), font::big);
    m_impl->healthLabel->setID("label");
    m_impl->healthLabel->setScale(0.375f);

    addChildAtPosition(m_impl->healthLabel, Anchor::Top, { 0.0f, m_impl->healthLabel->getScaledContentHeight() * -0.625f });

    scheduleUpdate();

    return true;
}

void JumpHealthMeter::update(float dt) {
    if (m_impl->healthMeter) {
        auto per = m_impl->healthMeter->getProgress();

        m_impl->healthMeter->updateProgress(per + (dt * m_impl->pace));

        if (m_impl->healthMeter->getProgress() >= 100.0f) {
            if (m_impl->jumps <= 25) m_impl->jumps++;

            m_impl->updateLabel();
            m_impl->healthMeter->updateProgress(0.0f);

            m_impl->pace = cw::rng::get(PACE_RANGES);
        };
    };
}

void JumpHealthMeter::jump() {
    if (m_impl->jumps <= 0) m_impl->callback();

    m_impl->jumps--;
    m_impl->updateLabel();
}

void JumpHealthMeter::reset() {
    unscheduleUpdate();

    m_impl->jumps = cw::rng::get(15, 7);

    if (m_impl->healthMeter) m_impl->healthMeter->updateProgress(0.0f);
    m_impl->updateLabel();

    scheduleUpdate();
}

void JumpHealthMeter::playPause(bool play) {
    play ? scheduleUpdate() : unscheduleUpdate();
}

JumpHealthMeter* JumpHealthMeter::create(Callback&& cb) {
    auto ret = new JumpHealthMeter();
    if (ret->init(std::move(cb))) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
}