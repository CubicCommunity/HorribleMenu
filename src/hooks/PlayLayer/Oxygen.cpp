#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static constexpr auto id = "oxygen";

inline static Option const o = {
    id,
    "Oxygen Level",
    "Limited oxygen level. You gain oxygen as a flying gamemode. If your oxygen runs out, the player dies.\n<cy>Credit: ArcticWoof</c>",
    category::playerlife,
    SillyTier::High,
};
HORRIBLE_REGISTER_OPTION(o);

class $modify(OxygenPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(id);

    struct Fields {
        bool withHealth = options::isEnabled("health");

        float m_oxygenLevel = 100.f;
        bool m_oxygenActive = false;

        ProgressBar* m_oxygenBar = nullptr;
        CCLabelBMFont* m_oxygenLabel = nullptr;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        auto f = m_fields.self();

        f->m_oxygenActive = true;
        f->m_oxygenLevel = 100.f;

        schedule(schedule_selector(OxygenPlayLayer::decreaseOxygen), 0.1f);

        if (!f->m_oxygenBar) {
            f->m_oxygenBar = ProgressBar::create();
            f->m_oxygenBar->setID("oxygen-bar"_spr);
            f->m_oxygenBar->setFillColor(colors::cyan);
            f->m_oxygenBar->setAnchorPoint({ 0.5f, 0.5f });
            f->m_oxygenBar->setPosition({ 10.f, getScaledContentHeight() / 2.f });
            f->m_oxygenBar->setRotation(-90.f);

            m_uiLayer->addChild(f->m_oxygenBar, 99);
        };

        f->m_oxygenBar->updateProgress(f->m_oxygenLevel);

        if (f->withHealth) f->m_oxygenBar->setPositionX(f->m_oxygenBar->getPositionX() + 25.f);

        auto const o2 = fmt::format("o2\n{}%", static_cast<int>(f->m_oxygenLevel));
        if (!f->m_oxygenLabel) {
            f->m_oxygenLabel = CCLabelBMFont::create(o2.c_str(), "bigFont.fnt");
            f->m_oxygenLabel->setColor(colors::cyan);
            f->m_oxygenLabel->setAnchorPoint({ 0.f, 1.f });
            f->m_oxygenLabel->setPosition({ 2.f, (getScaledContentHeight() / 2.f) - (f->m_oxygenBar->getScaledContentWidth() / 2.f) - 1.25f });
            f->m_oxygenLabel->setScale(0.25f);

            m_uiLayer->addChild(f->m_oxygenLabel, 100);
        } else {
            f->m_oxygenLabel->setString(o2.c_str());
        };

        f->m_oxygenLabel->setPosition({ f->m_oxygenBar->getPositionX() + 2.f - 10.f, (getScaledContentHeight() / 2.f) - (f->m_oxygenBar->getScaledContentWidth() / 2.f) - 1.25f });
    };

    void decreaseOxygen(float dt) {
        auto f = m_fields.self();

        if (!f->m_oxygenActive || !f->m_oxygenLabel) return;
        if (!m_player1) return;

        // regen o2
        if (m_player1->m_isBird || m_player1->m_isShip || m_player1->m_isSwing || m_player1->m_isDart) {
            f->m_oxygenLevel += 5.f * dt;
            // log::debug("Oxygen level increased: {}", f->m_oxygenLevel);
        } else {
            f->m_oxygenLevel -= 2.f * dt;
            // log::debug("Oxygen level decreased: {}", f->m_oxygenLevel);
        };

        if (f->m_oxygenLevel > 100.f) f->m_oxygenLevel = 100.f;
        if (f->m_oxygenLevel < 0.f) f->m_oxygenLevel = 0.f;

        auto const o2 = fmt::format("o2\n{}%", static_cast<int>(f->m_oxygenLevel));
        f->m_oxygenLabel->setString(o2.c_str());

        f->m_oxygenBar->updateProgress(f->m_oxygenLevel);

        // Destroy player if oxygen is 0
        if (f->m_oxygenLevel <= 0.f && m_player1 && !m_player1->m_isDead) destroyPlayer(m_player1, nullptr);
    };

    void resetOxygenLevel() {
        auto f = m_fields.self();

        f->m_oxygenLevel = 100.f;

        if (f->m_oxygenLabel) {
            auto const o2 = fmt::format("o2\n{}%", static_cast<int>(f->m_oxygenLevel));
            f->m_oxygenLabel->setString(o2.c_str());
        };
    };

    void resetLevel() {
        resetOxygenLevel();
        PlayLayer::resetLevel();
    };
};