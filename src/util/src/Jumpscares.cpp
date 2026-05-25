#include "../Jumpscares.hpp"

#include <Utils.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::util;

void jumpscares::switchLevel(LevelInfo const& level, bool dontCreateObjects, bool useReplay) {
    if (auto pl = PlayLayer::get()) {
        if (pl->m_level->m_levelID == level.id) return;
    };

    coro::getLevel(level.id, [level, dontCreateObjects, useReplay](Result<GJGameLevel*> result) {
        if (result.isOk()) {
            auto level = std::move(result).unwrap();

            if (auto mdm = MusicDownloadManager::sharedState()) {
                mdm->addMusicDownloadDelegate(jumpscares::JumpscareLevelManager::get());
                mdm->downloadSong(level->m_songID);
            };

            if (auto glm = GameLevelManager::sharedState()) glm->saveLevel(level);
            if (auto jm = jumpscares::JumpscareLevelManager::get()) jm->saveLevel(level);

            log::warn("Switching to {} level ({})", level->m_levelName, level->m_levelID.value());

            auto scene = PlayLayer::scene(level, useReplay, dontCreateObjects);
            CCDirector::sharedDirector()->replaceScene(scene);
        } else if (result.isErr()) {
            log::error("Failed to get level {}: {}", level.id, result.unwrapErr());
        };
    });
};

void jumpscares::coro::getLevel(int id, CopyableFunction<void(Result<GJGameLevel*>)>&& callback) {
    if (auto jm = jumpscares::JumpscareLevelManager::get()) {
        log::trace("Checking cache for level {}...", id);
        if (auto lvl = jm->getLevel(id)) return callback(Ok(lvl));
        log::debug("Level {} not found in cache, fetching...", id);
    };

    auto req = web::WebRequest()
                   .bodyString(fmt::format("secret=Wmfd2893gb7&levelID={}", id))
                   .userAgent("");

    log::trace("Preparing web request for level {} download", id);

    async::spawn(
        req.post("https://www.boomlings.com/database/downloadGJLevel22.php"),
        [cb = std::move(callback)](web::WebResponse res) {  // copyable has const () operator woohoo!
            auto const resStr = res.string().unwrapOrDefault();

            if (res.error() || resStr == "-1") {
                log::error("Error getting level data: {}", resStr);
                return cb(Err("An error occurred while fetching level data"));
            };

            auto dict = CCDictionary::create();
            auto splits = asp::iter::split(resStr, ":")
                              .mapCast<std::string>()
                              .collect();

            for (size_t i = 0; i + 1 < splits.size(); i += 2) dict->setObject(CCString::create(splits[i + 1]), splits[i]);
            cb(Ok(GJGameLevel::create(dict, false)));
        });
};

void jumpscares::JumpscareLevelManager::loadSongInfoFinished(SongInfoObject* object) {
    log::info("Song info loaded for song ID {}, saving level to cache if it exists...", object->m_songID);
};

void jumpscares::JumpscareLevelManager::loadSongInfoFailed(int id, GJSongError errorType) {
    log::error("Failed to load song info for song ID {}: error code {}", id, errorType == GJSongError::FailedToFetch ? "Failed to fetch" : "Unknown");
};

void jumpscares::JumpscareLevelManager::downloadSongStarted(int id) {
    log::trace("Download started for song ID {}", id);
};

void jumpscares::JumpscareLevelManager::downloadSongFinished(int id) {
    log::debug("Download finished for song ID {}", id);
};

void jumpscares::JumpscareLevelManager::saveLevel(GJGameLevel* level) {
    m_levels[level->m_levelID.value()] = level;
};

GJGameLevel* jumpscares::JumpscareLevelManager::getLevel(int id) const noexcept {
    auto it = m_levels.find(id);
    if (it != m_levels.end()) return it->second.data();

    return nullptr;
};

jumpscares::JumpscareLevelManager* jumpscares::JumpscareLevelManager::get() noexcept {
    static auto inst = new (std::nothrow) JumpscareLevelManager();
    return inst;
};