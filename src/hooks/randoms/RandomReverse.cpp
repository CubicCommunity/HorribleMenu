#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "random_reverse"

static auto const o = Option::create(THIS_ID)
                          ->setName("Random Reverses")
                          ->setDescription("Every time you jump, there's a chance your player might start to move in the opposite direction for a few seconds.\n<cl>created by Cheeseworks</c>")
                          ->setCategory(category::randoms)
                          ->setSillyTier(SillyTier::High)
                          ->setCheating(true)
                          ->autoRegister();

class $modify(RandomReverseGJBaseGameLayer, GJBaseGameLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        uint8_t chance = options::getChance(THIS_ID);

        bool reversed = false;
    };

    void handleButton(bool down, int button, bool isPlayer1) {
        GJBaseGameLayer::handleButton(down, button, isPlayer1);

        if (down && button == 1) {
            if (rng::chance(m_fields->chance)) reversePlayers();
        };
    };

    void resetPlayer() {
        GJBaseGameLayer::resetPlayer();

        unschedule(schedule_selector(RandomReverseGJBaseGameLayer::unreversePlayers));
        m_fields->reversed = false;
    };

    void reversePlayers() {
        if (!m_fields->reversed) {
            m_player1->reversePlayer(nullptr);
            m_player2->reversePlayer(nullptr);

            m_fields->reversed = true;

            scheduleOnce(schedule_selector(RandomReverseGJBaseGameLayer::unreversePlayers), rng::get(3.75f, 1.25f));
        };
    };

    void unreversePlayers(float) {
        if (m_fields->reversed) {
            if (!m_player1->m_isDead) m_player1->reversePlayer(nullptr);
            if (!m_player2->m_isDead) m_player2->reversePlayer(nullptr);

            m_fields->reversed = false;
        };
    };
};