#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "death"

static auto const o = Option::create(THIS_ID)
                          ->setName("Fake Death")
                          ->setDescription("The player's death effect will show without dying.\n<cl>suggested by DragonixGD</c>")
                          ->setCategory(category::obstructive)
                          ->setSillyTier(SillyTier::Medium)
                          ->autoRegister();

class $modify(FakeDeathPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields {
        bool m_dontCreateObjects = false;
        GameObject* m_destroyingObject;
    };

    void destroyPlayer(PlayerObject* player, GameObject* game) {
        auto f = m_fields.self();

        // Show explosion visual effect but do not kill the player

        // ignore the anti-cheat spike lmao
        if (game == m_anticheatSpike && player && !player->m_isDead) return PlayLayer::destroyPlayer(player, game);
        if (!f->m_destroyingObject) f->m_destroyingObject = game;

        sfx::play(sfx::file::bad);
        GJBaseGameLayer::shakeCamera(1.f, 2.f, 1.f);

        if (m_player1) {
            log::debug("fake death");
            m_player1->playDeathEffect();
            m_player1->resetPlayerIcon();

            m_player1->m_isDead = false;
        };

        if (m_player2) {
            log::debug("fake death");
            m_player2->playDeathEffect();
            m_player2->resetPlayerIcon();

            m_player2->m_isDead = false;
        };
    };
};