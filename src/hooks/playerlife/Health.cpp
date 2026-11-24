#include <Util.h>

#include <rhatt/prelude.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace cs::brkd::horrible::prelude;

#define THIS_ID "health"

static auto const o = Option::create(THIS_ID)
                          ->setName("Player Health")
                          ->setDescription("Add a health bar and decreases everytime you collide with a hazard. If your health reaches zero, the player dies.\n<cl>suggested by Cheeseworks</c>")
                          ->setCategory(category::playerlife)
                          ->setSillyTier(SillyTier::Low)
                          ->setCheating(true)
                          ->autoRegister();

class $modify(HealthPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        float health = 100.0f;

        ProgressBar* healthBar = nullptr;
        Label* healthLabel = nullptr;
    };

    HORRIBLE_SETUP_INTERFACE_FUNC {
        auto f = m_fields.self();

        if (!on) {
            cue::resetNode(f->healthBar);
            cue::resetNode(f->healthLabel);

            f->health = 100.0f;

            return;
        };

        if (!f->healthBar) {
            f->healthBar = ProgressBar::create();
            f->healthBar->setID("health-bar"_spr);
            f->healthBar->setFillColor(cw::colors::red);
            f->healthBar->setAnchorPoint(cw::anchor::center);
            f->healthBar->setPosition({ 10.0f, getScaledContentHeight() / 2.0f });
            f->healthBar->setRotation(-90.0f);

            m_uiLayer->addChild(f->healthBar, CW_HIGHEST_Z);
        };

        f->healthBar->updateProgress(f->health);

        auto hp = fmt::format("HP\n{}%", static_cast<uint8_t>(f->health));
        if (!f->healthLabel) {
            f->healthLabel = Label::create(std::move(hp), cw::font::big);
            f->healthLabel->setColor(cw::colors::red);
            f->healthLabel->setAnchorPoint({ 0.0f, 1.0f });
            f->healthLabel->setPosition({ 2.0f, (getScaledContentHeight() / 2.0f) - (f->healthBar->getScaledContentWidth() / 2.0f) - 1.25f });
            f->healthLabel->setScale(0.25f);

            m_uiLayer->addChild(f->healthLabel, CW_HIGHEST_Z);
        } else {
            f->healthLabel->setText(std::move(hp));
        };
    }

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        m_fields->health = 100.0f;

        HORRIBLE_SETUP_INTERFACE_FUNC_NAME();
    }

    void resetHealth() {
        auto f = m_fields.self();

        f->health = 100.0f;

        if (f->healthLabel) f->healthLabel->setText(fmt::format("HP\n{}%", static_cast<uint8_t>(f->health)));

        if (f->healthBar) f->healthBar->updateProgress(f->health);
    }

    void resetLevel() {
        resetHealth();
        PlayLayer::resetLevel();
    }

    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        auto f = m_fields.self();

        if (obj == m_anticheatSpike && !player->m_isDead) return PlayLayer::destroyPlayer(player, obj);

        if (f->health > 0) {
            f->health -= 0.125f;

            m_player1->playSpawnEffect();
            m_player2->playSpawnEffect();

            shakeCamera(1.25f, 2.5f, 0.00875F);

            if (f->healthLabel) {
                f->healthLabel->setText(fmt::format("HP\n{}%", static_cast<int>(f->health)));
                f->healthBar->updateProgress(f->health);
            };

            if (f->health <= 0.0f) {
                log::warn("Player health is dead: {}", f->health);
                PlayLayer::destroyPlayer(player, obj);
            };
        };
    }
};

HORRIBLE_TOGGLE_MODIFY(PlayLayer, HealthPlayLayer);