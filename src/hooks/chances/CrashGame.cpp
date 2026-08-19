#include <Util.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "crash_death"

static auto const o = Option::create(THIS_ID)
                          ->setName("Crash Chance on Death")
                          ->setDescription("When you die in a level, there's a chance your game will die too.\n<co>Your progress will save!</c> :)\n<cl>suggested by DragonixGD</c>")
                          ->setCategory(category::chances)
                          ->setSillyTier(SillyTier::High)
                          ->autoRegister();

class $modify(CrashGamePlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        uint8_t chance = options::getChance(THIS_ID);
    };

    void destroyPlayer(PlayerObject* p0, GameObject* p1) {
        auto f = m_fields.self();

        // ignore the anti-cheat spike lmao
        if (p1 == m_anticheatSpike && p0 && !p0->m_isDead) return;

        if (rng::chance(f->chance)) {
            log::warn("ur game crash hehehehehehehe");

            PlayLayer::destroyPlayer(p0, p1);
            game::exit(true);  // saves data mrrp
        };

        PlayLayer::destroyPlayer(p0, p1);
    };
};