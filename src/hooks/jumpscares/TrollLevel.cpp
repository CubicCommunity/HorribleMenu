#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "troll_level"

static auto const o = Option::create(THIS_ID)
                          ->setName("You're Getting Trolled")
                          ->setDescription("A chance when loading a level, to instead load a funny troll level.\n<cl>created by Cheeseworks</c>")
                          ->setCategory(category::jumpscares)
                          ->setSillyTier(SillyTier::High)
                          ->setOnline(true)
                          ->autoRegister();

namespace js_internal {
    static constexpr auto id = 57436521;

    static void saveTrollLevel() {
        jumpscares::coro::getLevel(id, [](Result<GJGameLevel*> result) {
            if (result.isOk()) {
                auto level = std::move(result).unwrap();

                if (auto mdm = MusicDownloadManager::sharedState()) {
                    mdm->addMusicDownloadDelegate(jumpscares::JumpscareLevelManager::get());
                    mdm->downloadSong(level->m_songID);
                };

                if (auto glm = GameLevelManager::sharedState()) glm->saveLevel(level);
                if (auto jm = jumpscares::JumpscareLevelManager::get()) jm->saveLevel(level);
            } else if (result.isErr()) {
                log::error("Failed to get level {}: {}", id, result.unwrapErr());
            };
        });
    };

    static void switchToTrollLevel(GJGameLevel* level, bool dontCreateObjects, bool useReplay) {
        log::warn("Switching to {} level ({})", level->m_levelName, level->m_levelID.value());

        auto scene = PlayLayer::scene(level, useReplay, dontCreateObjects);
        CCDirector::sharedDirector()->replaceScene(scene);
    };
};

$on_mod(Loaded) {
    if (options::isEnabled(THIS_ID)) js_internal::saveTrollLevel();

    listenForHorribleOptionChanges(
        THIS_ID,
        [](HorribleOptionSave data) {
            if (data.enabled) js_internal::saveTrollLevel();
        });
};

class $modify(TrollLevelPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        uint8_t chance = options::getChance(THIS_ID);
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (auto jm = jumpscares::JumpscareLevelManager::get()) {
            if (rng::fast() <= m_fields->chance) {
                if (auto lvl = jm->getLevel(js_internal::id)) return PlayLayer::init(lvl, useReplay, dontCreateObjects);
            };
        };

        return PlayLayer::init(level, useReplay, dontCreateObjects);
    };
};