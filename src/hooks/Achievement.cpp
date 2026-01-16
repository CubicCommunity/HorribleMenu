#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/CCMenuItem.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class $modify(AchievementCCMenuItem, CCMenuItem) {
    struct Fields {
        bool enabled = options::get(key::achieve);
        int chance = options::getChance(key::achieve);
    };

    void activate() {
        CCMenuItem::activate();

        auto f = m_fields.self();

        this->template addEventListener<OptionEventFilter>(
            [this, f](OptionEvent* ev) {
                f->enabled = ev->getToggled();
                return ListenerResult::Propagate;
            },
            key::achieve
        );

        if (f->enabled) {
            if (auto fmod = FMODAudioEngine::sharedEngine()) {
                int rnd = randng::fast();
                log::debug("button menu chance {}", rnd);

                // @geode-ignore(unknown-resource)
                if (rnd <= f->chance) fmod->playEffectAsync("achievement_01.ogg");
            };
        };
    };
};