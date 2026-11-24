#include <Util.h>

#include <rhatt/prelude.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace cs::brkd::horrible::prelude;

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
        float oxygenLevel = 50.0f;

        ProgressBar* oxygenBar = nullptr;
        Label* oxygenLabel = nullptr;
    };

    HORRIBLE_SETUP_INTERFACE_FUNC {
        auto f = m_fields.self();

        if (!on) {
            cue::resetNode(f->oxygenBar);
            cue::resetNode(f->oxygenLabel);

            unschedule(schedule_selector(OxygenPlayLayer::decreaseOxygen));

            f->oxygenLevel = 50.0f;

            return;
        };

        schedule(schedule_selector(OxygenPlayLayer::decreaseOxygen), 0.125f);

        if (!f->oxygenBar) {
            f->oxygenBar = ProgressBar::create();
            f->oxygenBar->setID("oxygen-bar"_spr);
            f->oxygenBar->setFillColor(cw::colors::cyan);
            f->oxygenBar->setAnchorPoint(cw::anchor::center);
            f->oxygenBar->setPosition({ 10.0f, getScaledContentHeight() / 2.0f });
            f->oxygenBar->setRotation(-90.0f);

            m_uiLayer->addChild(f->oxygenBar, CW_HIGHEST_Z);
        };

        f->oxygenBar->updateProgress(f->oxygenLevel);

        auto o2 = fmt::format("o2\n{}%", static_cast<uint8_t>(f->oxygenLevel));
        if (!f->oxygenLabel) {
            f->oxygenLabel = Label::create(std::move(o2), font::big);
            f->oxygenLabel->setColor(cw::colors::cyan);
            f->oxygenLabel->setAnchorPoint({ 0.0f, 1.0f });
            f->oxygenLabel->setPosition({ 2.0f, (getScaledContentHeight() / 2.0f) - (f->oxygenBar->getScaledContentWidth() / 2.0f) - 1.25f });
            f->oxygenLabel->setScale(0.25f);

            m_uiLayer->addChild(f->oxygenLabel, CW_HIGHEST_Z);
        } else {
            f->oxygenLabel->setText(std::move(o2));
        };

        reposOxygenBar(options::isEnabled(g_healthID));
    }

    void reposOxygenBar(bool healthOn) {
        auto f = m_fields.self();

        if (f->oxygenBar) f->oxygenBar->setPositionX(10.0f + (healthOn ? 25.0f : 0.0f));
        if (f->oxygenLabel) f->oxygenLabel->setPosition({ f->oxygenBar->getPositionX() + 2.0f - 10.0f, (getScaledContentHeight() / 2.0f) - (f->oxygenBar->getScaledContentWidth() / 2.0f) - 1.25f });
    }

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        m_fields->oxygenLevel = 50.0f;

        HORRIBLE_SETUP_INTERFACE_FUNC_NAME();
    }

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

        if (f->oxygenLevel > 100.0f) f->oxygenLevel = 100.0f;
        if (f->oxygenLevel < 0.0f) f->oxygenLevel = 0.0f;

        f->oxygenLabel->setText(fmt::format("o2\n{}%", static_cast<uint8_t>(f->oxygenLevel)));

        f->oxygenBar->updateProgress(f->oxygenLevel);

        // Destroy player if oxygen is 0
        if (f->oxygenLevel <= 0.0f && !m_playerDied) destroyPlayer(m_player1, nullptr);
    }

    void resetOxygenLevel() {
        auto f = m_fields.self();

        f->oxygenLevel = 50.0f;

        if (f->oxygenLabel) f->oxygenLabel->setText(fmt::format("o2\n{}%", static_cast<int>(f->oxygenLevel)));
    }

    void resetLevel() {
        resetOxygenLevel();
        PlayLayer::resetLevel();
    }
};

$on_mod(Loaded) {
    HORRIBLE_MODIFY_EVENT_HANDLER(PlayLayer, OxygenPlayLayer);

    listenForHorribleOptionChanges(
        g_healthID,
        [](OptionSave data) {
            if (auto pl = PlayLayer::get()) modify_cast<OxygenPlayLayer*>(pl)->reposOxygenBar(data.enabled);
        }
    );
}