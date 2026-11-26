#include <Util.h>

#include <rhatt/prelude.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace cs::brkd::horrible::prelude;

#define THIS_ID "troll_level"

static auto const o = Option::create(THIS_ID)
                          ->setName("You're Getting Trolled")
                          ->setDescription("A chance when loading a level, to instead load a funny troll level.\n<cl>created by Cheeseworks</c>")
                          ->setCategory(category::jumpscares)
                          ->setSillyTier(SillyTier::High)
                          ->setOnline(true)
                          ->autoRegister();

namespace js_internal {
    static void saveTrollLevel() {
        jumpscares::downloadLevel(HORRIBLE_JUMPSCARES_TROLL, [](Ref<GJGameLevel> lvl) { log::info("Troll level '{}' downloaded and saved!", lvl->m_levelName); });
    }

    static void switchToTrollLevel(GJGameLevel* level, bool dontCreateObjects) {
        log::warn("Switching to {} level ({})", level->m_levelName, level->m_levelID.value());
        CCDirector::sharedDirector()->replaceScene(PlayLayer::scene(level, dontCreateObjects));
    }
};

$on_mod(Loaded) {
    if (options::isEnabled(THIS_ID)) js_internal::saveTrollLevel();

    listenForHorribleOptionChanges(
        THIS_ID,
        [](OptionSave data) {
            if (data.enabled) js_internal::saveTrollLevel();
        }
    );
}

class $modify(TrollLevelPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        uint8_t chance = options::getChance(THIS_ID);
    };

    bool init(GJGameLevel* level, bool dontCreateObjects) {
        if (cw::rng::chance(m_fields->chance)) {
            if (auto lvl = GameLevelManager::sharedState()->getSavedLevel(HORRIBLE_JUMPSCARES_TROLL)) return PlayLayer::init(lvl, dontCreateObjects);
        };

        return PlayLayer::init(level, dontCreateObjects);
    }
};