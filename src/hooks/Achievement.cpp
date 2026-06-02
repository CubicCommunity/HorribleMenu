#include <Utils.h>
#include <horrible/API.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/CCMenuItem.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "achieve"

static auto const o = Option::create(THIS_ID)
                          ->setName("Random Achievements")
                          ->setDescription("Randomly play the achievement sound when clicking buttons.\n<cl>suggested by Cheeseworks</c>")
                          ->setCategory(category::misc)
                          ->setSillyTier(SillyTier::Low)
                          ->autoRegister();

class $modify(AchievementCCMenuItem, CCMenuItem) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        uint8_t chance = options::getChance(THIS_ID);
    };

    void activate() {
        CCMenuItem::activate();

        // @geode-ignore(unknown-resource)
        if (rng::chance(m_fields->chance)) sfx::play("achievement_01.ogg");
    };
};