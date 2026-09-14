#include "../Jumpscares.hpp"

#include <argon/argon.hpp>

#include <Util.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::util;

void jumpscares::switchLevel(int level, bool dontCreateObjects, bool useReplay, CopyableFunction<void()>&& callback) {
    if (auto pl = PlayLayer::get()) {
        if (pl->m_level->m_levelID == level) return;
    };

    async::spawn(
        coro::getLevel(level),
        [level, dontCreateObjects, useReplay, cb = std::move(callback)](coro::LevelResult result) {
            if (result.isOk()) {
                auto lvl = std::move(result).unwrap();

                if (auto jm = jumpscares::JumpscareLevelManager::get()) jm->saveLevel(lvl);

                log::warn("Switching to {} level ({})", lvl->m_levelName, lvl->m_levelID.value());
                CCDirector::sharedDirector()->replaceScene(PlayLayer::scene(lvl, useReplay, dontCreateObjects));

                if (cb) cb();
            } else if (result.isErr()) {
                log::error("Failed to get level {}: {}", level, result.unwrapErr());
            };
        });
};

jumpscares::coro::LevelFuture jumpscares::coro::getLevel(int id) {
    auto cache = *co_await async::waitForMainThread<LevelResult>([id]() -> LevelResult {
        if (auto jm = jumpscares::JumpscareLevelManager::get()) {
            log::trace("Checking cache for level {}...", id);
            if (auto lvl = jm->getLevel(id)) return Ok(lvl);

            log::debug("Level {} not found in cache, fetching...", id);
            return Err("Level not found in cache");
        };

        return Err("Unknown error");
    });
    if (cache.isOk()) co_return cache;

    auto auth = *co_await async::waitForMainThread<std::string>([]() {
        if (argon::signedIn()) {
            auto const acc = argon::getGameAccountData();
            return fmt::format("&accountID={}&gjp2={}", acc.accountId, acc.gjp2);
        };

        return std::string{};
    });

    auto req = request::base()
                   .bodyString(fmt::format("secret=Wmfd2893gb7&levelID={}{}", id, auth))
                   .userAgent("");  // robby why do you hate user agents and not ai agents

    log::trace("Preparing web request for level {} download", id);

    auto res = co_await req.post("https://www.boomlings.com/database/downloadGJLevel22.php");

    auto strRes = res.string();
    if (strRes.isErr()) co_return Err(std::move(strRes).unwrapErr());

    auto str = std::move(strRes).unwrap();

    if (res.error() || str == "-1") {
        log::error("Error getting level data: {}", str);
        co_return Err("An error occurred while fetching level data");
    };

    auto result = *co_await async::waitForMainThread<LevelResult>([lvl = std::move(str)]() -> LevelResult {
        auto dict = CCDictionary::create();
        auto splits = asp::iter::split(lvl, ":").collect();

        for (size_t i = 0; i + 1 < splits.size(); i += 2) dict->setObject(CCString::create(std::string{splits[i + 1]}), std::string{splits[i]});
        return Ok(GJGameLevel::create(dict, false));
    });
    co_return result;
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
    if (auto const it = m_levels.find(id); it != m_levels.end()) return it->second.data();
    return nullptr;
};