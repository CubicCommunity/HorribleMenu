#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static Option const o = {
        "crash_death",
        "Crash Chance on Death",
        "When you die in a level, there's a chance your game will die too.\n<cg>Don't worry, your progress will save when crashing!</c> :)\n<cy>Credit: DragonixGD</c>",
        category::chances,
        SillyTier::High,
};
REGISTER_HORRIBLE_OPTION(o);

class $modify(CrashGamePlayLayer, PlayLayer) {
    struct Fields {
        bool enabled = options::get(o.id);
        int chance = options::getChance(o.id);
    };

    void destroyPlayer(PlayerObject * p0, GameObject * p1) {
        auto f = m_fields.self();

        if (f->enabled) {
            // ignore the anti-cheat spike lmao
            if (p1 == m_anticheatSpike && p0 && !p0->m_isDead) return;

            int rnd = randng::fast();
            log::debug("crash destroy chance {}", rnd);

            if (rnd <= f->chance) {
                log::warn("ur game crash hehehehehehehe");

                PlayLayer::destroyPlayer(p0, p1);
                game::exit(true); // saves data
            };
        };

        PlayLayer::destroyPlayer(p0, p1);
    };
};