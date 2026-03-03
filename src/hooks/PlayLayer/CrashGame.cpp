#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static constexpr auto id = "crash_death";

inline static Option const o = {
    id,
    "Crash Chance on Death",
    "When you die in a level, there's a chance your game will die too.\n<cg>Don't worry, your progress will save when crashing!</c> :)\n<cy>Credit: DragonixGD</c>",
    category::chances,
    SillyTier::High,
};
HORRIBLE_REGISTER_OPTION(o);

class $modify(CrashGamePlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(id);

    struct Fields {
        int chance = options::getChance(id);
    };

    void destroyPlayer(PlayerObject * p0, GameObject * p1) {
        auto f = m_fields.self();

        // ignore the anti-cheat spike lmao
        if (p1 == m_anticheatSpike && p0 && !p0->m_isDead) return;

        int rnd = randng::fast();
        log::trace("crash destroy chance {}", rnd);

        if (rnd <= f->chance) {
            log::warn("ur game crash hehehehehehehe");

            PlayLayer::destroyPlayer(p0, p1);
            game::exit(true); // saves data
        };

        PlayLayer::destroyPlayer(p0, p1);
    };
};