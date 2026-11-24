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
        async::spawn(
            jumpscares::coro::getLevel(HORRIBLE_JUMPSCARES_TROLL),
            [](jumpscares::coro::LevelResult result) {
                if (result.isOk()) {
                    if (auto jm = jumpscares::JumpscareLevelManager::get()) jm->saveLevel(std::move(result).unwrap());
                } else if (result.isErr()) {
                    log::error("Failed to get level {}: {}", HORRIBLE_JUMPSCARES_TROLL, result.unwrapErr());
                };
            }
        );
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
        if (auto jm = jumpscares::JumpscareLevelManager::get()) {
            if (cw::rng::chance(m_fields->chance)) {
                if (auto lvl = jm->getLevel(HORRIBLE_JUMPSCARES_TROLL)) return PlayLayer::init(lvl, dontCreateObjects);
            };
        };

        return PlayLayer::init(level, dontCreateObjects);
    }
};