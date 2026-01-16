#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class $modify(CrashGamePlayLayer, PlayLayer) {
    struct Fields {
        bool enabled = options::get(key::crash_death);
        int chance = options::getChance(key::crash_death);
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