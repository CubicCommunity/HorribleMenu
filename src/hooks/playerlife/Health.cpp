#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "health"

static auto const o = Option::create(THIS_ID)
                          ->setName("Player Health")
                          ->setDescription("Add a health bar and decreases everytime you collide with a hazard. If your health reaches zero, the player dies.\n<cl>suggested by Cheeseworks</c>")
                          ->setCategory(category::playerlife)
                          ->setSillyTier(SillyTier::Medium)
                          ->setCheating(true)
                          ->autoRegister();

class $modify(HealthPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        float health = 100.f;

        ProgressBar* healthBar = nullptr;
        CCLabelBMFont* healthLabel = nullptr;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        auto f = m_fields.self();

        f->health = 100.f;

        if (!f->healthBar) {
            f->healthBar = ProgressBar::create();
            f->healthBar->setID("health-bar"_spr);
            f->healthBar->setFillColor(colors::red);
            f->healthBar->setAnchorPoint(anchor::center);
            f->healthBar->setPosition({10.f, getScaledContentHeight() / 2.f});
            f->healthBar->setRotation(-90.f);

            m_uiLayer->addChild(f->healthBar, HIGHEST_Z);
        };

        f->healthBar->updateProgress(f->health);

        auto const hp = fmt::format("HP\n{}%", static_cast<int>(f->health));
        if (!f->healthLabel) {
            f->healthLabel = CCLabelBMFont::create(hp.c_str(), "bigFont.fnt");
            f->healthLabel->setColor(colors::red);
            f->healthLabel->setAnchorPoint({0.f, 1.f});
            f->healthLabel->setPosition({2.f, (getScaledContentHeight() / 2.f) - (f->healthBar->getScaledContentWidth() / 2.f) - 1.25f});
            f->healthLabel->setScale(0.25f);

            m_uiLayer->addChild(f->healthLabel, HIGHEST_Z);
        } else {
            f->healthLabel->setString(hp.c_str());
        };
    };

    void resetHealth() {
        auto f = m_fields.self();

        f->health = 100.f;

        if (f->healthLabel) {
            auto const hp = fmt::format("HP\n{}%", static_cast<int>(f->health));
            f->healthLabel->setString(hp.c_str());
        };

        if (f->healthBar) f->healthBar->updateProgress(f->health);
    };

    void resetLevel() {
        resetHealth();
        PlayLayer::resetLevel();
    };

    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        auto f = m_fields.self();

        if (obj == m_anticheatSpike && !player->m_isDead) return PlayLayer::destroyPlayer(player, obj);

        auto rnd = rng::fast();
        if (f->health > 0) {
            f->health -= 0.125f;

            m_player1->playSpawnEffect();
            m_player2->playSpawnEffect();

            shakeCamera(1.25f, 2.5f, 0.00875F);

            if (f->healthLabel) {
                auto const hp = fmt::format("HP\n{}%", static_cast<int>(f->health));
                f->healthLabel->setString(hp.c_str());

                f->healthBar->updateProgress(f->health);
            };

            if (f->health <= 0.f) {
                log::warn("Player health is dead: {}", f->health);
                PlayLayer::destroyPlayer(player, obj);
            };
        };
    };
};