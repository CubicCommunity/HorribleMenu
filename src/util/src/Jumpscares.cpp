#include "../Jumpscares.hpp"

#include <Util.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::util;

void jumpscares::switchLevel(int level, bool dontCreateObjects, bool useReplay, CopyableFunction<void()>&& callback) {
    if (auto pl = PlayLayer::get()) {
        if (pl->m_level->m_levelID == level) return;
    };

    coro::getLevel(level, [level, dontCreateObjects, useReplay, cb = std::move(callback)](Result<GJGameLevel*> result) {
        if (result.isOk()) {
            auto lvl = std::move(result).unwrap();

            if (auto jm = jumpscares::JumpscareLevelManager::get()) jm->saveLevel(lvl);

            log::warn("Switching to {} level ({})", lvl->m_levelName, lvl->m_levelID.value());

            auto scene = PlayLayer::scene(lvl, useReplay, dontCreateObjects);
            CCDirector::sharedDirector()->replaceScene(scene);

            if (cb) cb();
        } else if (result.isErr()) {
            log::error("Failed to get level {}: {}", level, result.unwrapErr());
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
            auto splits = asp::iter::split(resStr, ":").collect();

            for (size_t i = 0; i + 1 < splits.size(); i += 2) dict->setObject(CCString::create(std::string{splits[i + 1]}), std::string{splits[i]});
            cb(Ok(GJGameLevel::create(dict, false)));
        });
};

void jumpscares::JumpscareLevelManager::saveLevel(GJGameLevel* level) {
    if (auto mdm = MusicDownloadManager::sharedState()) {
        // mdm->addMusicDownloadDelegate(jumpscares::JumpscareLevelManager::get());
        mdm->downloadSong(level->m_songID);
    };

    if (auto glm = GameLevelManager::sharedState()) glm->saveLevel(level);

    m_levels[level->m_levelID.value()] = level;
};

GJGameLevel* jumpscares::JumpscareLevelManager::getLevel(int id) const noexcept {
    auto it = m_levels.find(id);
    if (it != m_levels.end()) return it->second.data();

    return nullptr;
};