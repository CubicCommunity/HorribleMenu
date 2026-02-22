#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static constexpr auto id = "health";

inline static Option const o = {
    id,
    "Player Health",
    "Add a health bar and decreases everytime you collide with a hazard. If your health reaches zero, the player dies.\n<cy>Credit: Cheeseworks</c>",
    category::playerlife,
    SillyTier::Medium,
};
HORRIBLE_REGISTER_OPTION(o);

class $modify(HealthPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(id);

    struct Fields {
        float m_health = 100.f;

        ProgressBar* m_healthBar = nullptr;
        CCLabelBMFont* m_healthLabel = nullptr;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        auto f = m_fields.self();

        f->m_health = 100.f;

        if (!f->m_healthBar) {
            f->m_healthBar = ProgressBar::create();
            f->m_healthBar->setID("health-bar"_spr);
            f->m_healthBar->setFillColor(colors::red);
            f->m_healthBar->setAnchorPoint({ 0.5f, 0.5f });
            f->m_healthBar->setPosition({ 10.f, getScaledContentHeight() / 2.f });
            f->m_healthBar->setRotation(-90.f);

            m_uiLayer->addChild(f->m_healthBar, 99);
        };

        f->m_healthBar->updateProgress(f->m_health);

        auto const hp = fmt::format("HP\n{}%", static_cast<int>(f->m_health));
        if (!f->m_healthLabel) {
            f->m_healthLabel = CCLabelBMFont::create(hp.c_str(), "bigFont.fnt");
            f->m_healthLabel->setColor(colors::red);
            f->m_healthLabel->setAnchorPoint({ 0.f, 1.f });
            f->m_healthLabel->setPosition({ 2.f, (getScaledContentHeight() / 2.f) - (f->m_healthBar->getScaledContentWidth() / 2.f) - 1.25f });
            f->m_healthLabel->setScale(0.25f);

            m_uiLayer->addChild(f->m_healthLabel, 100);
        } else {
            f->m_healthLabel->setString(hp.c_str());
        };
    };

    void resetHealth() {
        auto f = m_fields.self();

        f->m_health = 100.f;

        if (f->m_healthLabel) {
            auto const hp = fmt::format("HP\n{}%", static_cast<int>(f->m_health));
            f->m_healthLabel->setString(hp.c_str());
        };

        if (f->m_healthBar) f->m_healthBar->updateProgress(f->m_health);
    };

    void resetLevel() {
        resetHealth();
        PlayLayer::resetLevel();
    };

    void destroyPlayer(PlayerObject * player, GameObject * game) {
        auto f = m_fields.self();

        // ignore the anti-cheat spike lmao
        if (game == m_anticheatSpike && player && !player->m_isDead) return;

        int rnd = randng::fast();
        if (f->m_health > 0) {
            f->m_health -= 0.1f;

            // log::debug("Player health is {}", f->m_health);

            m_player1->playSpawnEffect();
            m_player2->playSpawnEffect();

            shakeCamera(1.25f, 2.5f, 0.00875F);

            if (f->m_healthLabel) {
                auto const hp = fmt::format("HP\n{}%", static_cast<int>(f->m_health));
                f->m_healthLabel->setString(hp.c_str());

                f->m_healthBar->updateProgress(f->m_health);
            };

            if (f->m_health <= 0.f) {
                log::warn("Player health is dead: {}", f->m_health);
                PlayLayer::destroyPlayer(player, game);
            };
        };
    };
};