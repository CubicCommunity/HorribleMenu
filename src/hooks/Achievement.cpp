#include <Utils.hpp>
#include <Horrible.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/CCMenuItem.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static constexpr auto id = "achieve";

inline static Option const o = {
    id,
    "Random Achievements",
    "Randomly play the achievement sound when clicking buttons.\n<cl>Credit: Cheeseworks</c>",
    category::randoms,
    SillyTier::Low};
HORRIBLE_REGISTER_OPTION(o);

class $modify(AchievementCCMenuItem, CCMenuItem) {
    HORRIBLE_DELEGATE_HOOKS(id);

    struct Fields {
        int chance = options::getChance(id);
    };

    void activate() {
        CCMenuItem::activate();

        auto f = m_fields.self();

        int rnd = randng::fast();
        log::trace("button menu chance {}", rnd);

        // @geode-ignore(unknown-resource)
        if (rnd <= f->chance) sfx::play("achievement_01.ogg");
    };
};