#include <Utils.h>
#include <Horrible.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/CCMenuItem.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

static constexpr auto id = "achieve";

static auto const o = Option::create(id)
                          .setName("Random Achievements")
                          .setDescription("Randomly play the achievement sound when clicking buttons.\n<cl>suggested by Cheeseworks</c>")
                          .setCategory(category::randoms)
                          .setSillyTier(SillyTier::Low);
HORRIBLE_REGISTER_OPTION(o);

class $modify(AchievementCCMenuItem, CCMenuItem) {
    HORRIBLE_DELEGATE_HOOKS(id);

    struct Fields {
        unsigned int chance = options::getChance(id);
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