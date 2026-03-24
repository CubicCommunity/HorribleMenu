#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

static constexpr auto id = "crash_death";

static auto const o = Option::create(id)
                          .setName("Crash Chance on Death")
                          .setDescription("When you die in a level, there's a chance your game will die too.\n<co>Your progress will save!</c> :)\n<cl>Credit: DragonixGD</c>")
                          .setCategory(category::chances)
                          .setSillyTier(SillyTier::High);
HORRIBLE_REGISTER_OPTION(o);

class $modify(CrashGamePlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(id);

    struct Fields {
        unsigned int chance = options::getChance(id);
    };

    void destroyPlayer(PlayerObject* p0, GameObject* p1) {
        auto f = m_fields.self();

        // ignore the anti-cheat spike lmao
        if (p1 == m_anticheatSpike && p0 && !p0->m_isDead) return;

        int rnd = randng::fast();
        log::trace("crash destroy chance {}", rnd);

        if (rnd <= f->chance) {
            log::warn("ur game crash hehehehehehehe");

            PlayLayer::destroyPlayer(p0, p1);
            game::exit(true);  // saves data
        };

        PlayLayer::destroyPlayer(p0, p1);
    };
};