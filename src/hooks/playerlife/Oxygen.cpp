#include <Util.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "oxygen"

static constexpr auto g_healthID = "health";

static auto const o = Option::create(THIS_ID)
                          ->setName("Oxygen Level")
                          ->setDescription("Limited oxygen level for the player. If oxygen runs out, the player dies. Oxygen usage increases while you hold down jump. You re-gain oxygen as a flying gamemode.\n<cl>suggested by ArcticWoof</c>")
                          ->setCategory(category::playerlife)
                          ->setSillyTier(SillyTier::High)
                          ->autoRegister();

class $modify(OxygenPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        float oxygenLevel = 50.f;

        ProgressBar* oxygenBar = nullptr;
        CCLabelBMFont* oxygenLabel = nullptr;
    };

    HORRIBLE_SETUP_INTERFACE_FUNC {
        auto f = m_fields.self();

        if (!on) {
            cue::resetNode(f->oxygenBar);
            cue::resetNode(f->oxygenLabel);

            unschedule(schedule_selector(OxygenPlayLayer::decreaseOxygen));

            f->oxygenLevel = 50.f;

            return;
        };

        schedule(schedule_selector(OxygenPlayLayer::decreaseOxygen), 0.125f);

        if (!f->oxygenBar) {
            f->oxygenBar = ProgressBar::create();
            f->oxygenBar->setID("oxygen-bar"_spr);
            f->oxygenBar->setFillColor(colors::cyan);
            f->oxygenBar->setAnchorPoint(anchor::center);
            f->oxygenBar->setPosition({10.f, getScaledContentHeight() / 2.f});
            f->oxygenBar->setRotation(-90.f);

            m_uiLayer->addChild(f->oxygenBar, HIGHEST_Z);
        };

        f->oxygenBar->updateProgress(f->oxygenLevel);

        auto const o2 = fmt::format("o2\n{}%", static_cast<uint8_t>(f->oxygenLevel));
        if (!f->oxygenLabel) {
            f->oxygenLabel = CCLabelBMFont::create(o2.c_str(), font::big);
            f->oxygenLabel->setColor(colors::cyan);
            f->oxygenLabel->setAnchorPoint({0.f, 1.f});
            f->oxygenLabel->setPosition({2.f, (getScaledContentHeight() / 2.f) - (f->oxygenBar->getScaledContentWidth() / 2.f) - 1.25f});
            f->oxygenLabel->setScale(0.25f);

            m_uiLayer->addChild(f->oxygenLabel, HIGHEST_Z);
        } else {
            f->oxygenLabel->setString(o2.c_str());
        };

        reposOxygenBar(options::isEnabled(g_healthID));
    };

    void reposOxygenBar(bool healthOn) {
        auto f = m_fields.self();

        if (f->oxygenBar) f->oxygenBar->setPositionX(10.f + (healthOn ? 25.f : 0.f));
        if (f->oxygenLabel) f->oxygenLabel->setPosition({f->oxygenBar->getPositionX() + 2.f - 10.f, (getScaledContentHeight() / 2.f) - (f->oxygenBar->getScaledContentWidth() / 2.f) - 1.25f});
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        m_fields->oxygenLevel = 50.f;

        HORRIBLE_SETUP_INTERFACE_FUNC_NAME();
    };

    void decreaseOxygen(float dt) {
        if (m_player1->m_isDead) return;

        auto f = m_fields.self();

        if (m_player1->m_jumpBuffered) f->oxygenLevel -= 0.125f;

        // regen o2
        if (m_player1->m_isBird || m_player1->m_isShip || m_player1->m_isSwing || m_player1->m_isDart) {
            f->oxygenLevel += 3.75f * dt;
        } else {
            f->oxygenLevel -= 2.5f * dt;
            if (!m_player1->m_isOnGround) f->oxygenLevel -= 0.5f * dt;
        };

        if (f->oxygenLevel > 100.f) f->oxygenLevel = 100.f;
        if (f->oxygenLevel < 0.f) f->oxygenLevel = 0.f;

        auto const o2 = fmt::format("o2\n{}%", static_cast<uint8_t>(f->oxygenLevel));
        f->oxygenLabel->setString(o2.c_str());

        f->oxygenBar->updateProgress(f->oxygenLevel);

        // Destroy player if oxygen is 0
        if (f->oxygenLevel <= 0.f && !m_playerDied) destroyPlayer(m_player1, nullptr);
    };

    void resetOxygenLevel() {
        auto f = m_fields.self();

        f->oxygenLevel = 50.f;

        if (f->oxygenLabel) {
            auto const o2 = fmt::format("o2\n{}%", static_cast<int>(f->oxygenLevel));
            f->oxygenLabel->setString(o2.c_str());
        };
    };

    void resetLevel() {
        resetOxygenLevel();
        PlayLayer::resetLevel();
    };
};

$on_mod(Loaded) {
    HORRIBLE_MODIFY_EVENT_HANDLER(PlayLayer, OxygenPlayLayer);

    listenForHorribleOptionChanges(
        g_healthID,
        [](HorribleOptionSave data) {
            if (auto pl = PlayLayer::get()) modify_cast<OxygenPlayLayer*>(pl)->reposOxygenBar(data.enabled);
        });
};