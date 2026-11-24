#include <Util.h>

#include <rhatt/prelude.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace cs::brkd::horrible::prelude;

#define THIS_ID "death"

static auto const o = Option::create(THIS_ID)
                          ->setName("Fake Death")
                          ->setDescription("The player's death effect will show without dying.\n<cl>suggested by DragonixGD</c>")
                          ->setCategory(category::obstructive)
                          ->setSillyTier(SillyTier::Low)
                          ->setCheating(true)
                          ->autoRegister();

class $modify(FakeDeathPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    void destroyPlayer(PlayerObject* player, GameObject* game) {
        if (game == m_anticheatSpike) return PlayLayer::destroyPlayer(player, game);

        sfx::play(sfx::file::bad);
        GJBaseGameLayer::shakeCamera(1.0f, 2.0f, 1.0f);

        playDeath(m_player1);
        playDeath(m_player2);
    }

    void playDeath(PlayerObject* player) {
        if (player) {
            player->playDeathEffect();
            player->resetPlayerIcon();

            player->m_isDead = false;
        };
    }
};