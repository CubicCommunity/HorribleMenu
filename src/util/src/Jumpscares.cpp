#include "../Jumpscares.hpp"

#include <Utils.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::util;

jumpscares::util::DownloadDelegate::DownloadDelegate(PlayLayer* pl, int levelId, std::string levelName, bool dontCreateObjects, bool useReplay) :
    m_playLayer(pl), m_levelId(levelId), m_levelName(std::move(levelName)), m_dontCreateObjects(dontCreateObjects), m_useReplay(useReplay) {}

void jumpscares::util::DownloadDelegate::levelDownloadFinished(GJGameLevel* level) {
    log::trace("Download finished for level {}", m_levelId);

    if (!level || level->m_levelID.value() != m_levelId) {
        log::error("Downloaded level mismatch or null: expected {}, got {}", m_levelId, level ? level->m_levelID.value() : -1);
        clearDownloadDelegate(this);
        delete this;
        return;
    };

    log::debug("Delegate cleared, checking level validity");
    clearDownloadDelegate(this);

    if (level->m_levelNotDownloaded) {
        log::error("Downloaded level {} is still not ready", m_levelId);
        delete this;
        return;
    };

    if (m_playLayer.lock()) {
        log::warn("Downloaded and switching to {} level ({})", m_levelName, m_levelId);
        auto scene = PlayLayer::scene(level, m_useReplay, m_dontCreateObjects);
        CCDirector::sharedDirector()->replaceScene(scene);
    } else {
        log::warn("PlayLayer weak pointer expired, cannot switch scene for level {}", m_levelName);
    };

    delete this;
};

void jumpscares::util::DownloadDelegate::levelDownloadFailed(int response) {
    clearDownloadDelegate(this);
    log::error("Failed to download {} ({})", m_levelName, response);
    delete this;
};

jumpscares::util::SearchDelegate::SearchDelegate(PlayLayer* pl, int levelId, std::string levelName, int songId, bool dontCreateObjects, bool useReplay) :
    m_playLayer(pl), m_levelId(levelId), m_levelName(std::move(levelName)), m_songId(songId), m_dontCreateObjects(dontCreateObjects), m_useReplay(useReplay) {};

void jumpscares::util::SearchDelegate::loadLevelsFinished(CCArray* levels, char const* key) {
    log::trace("Search finished for level {}", m_levelName);
    util::clearLevelManagerDelegate(this);

    if (!levels || levels->count() == 0) {
        log::error("No online level result returned for {}", m_levelName);
        delete this;
        return;
    };

    if (auto level = typeinfo_cast<GJGameLevel*>(levels->objectAtIndex(0))) {
        log::debug("Level object retrieved from search, starting download for {}", m_levelName);
        auto delegate = new jumpscares::util::DownloadDelegate(m_playLayer.lock().take(), m_levelId, m_levelName, m_dontCreateObjects, m_useReplay);
        util::downloadLevelWithDelegate(level->m_levelID.value(), m_songId, delegate);
    } else {
        log::error("Online search returned non-level object for {}", m_levelName);
        delete this;
    };
};

void jumpscares::util::SearchDelegate::loadLevelsFailed(char const* key) {
    util::clearLevelManagerDelegate(this);
    log::error("Failed to fetch online level info for {}", m_levelName);
    delete this;
};

GJGameLevel* jumpscares::util::getSavedDownloadedLevel(int levelId) {
    if (auto glm = GameLevelManager::sharedState()) {
        if (!glm->hasDownloadedLevel(levelId)) return nullptr;
        return glm->getSavedLevel(levelId);
    };

    log::error("GameLevelManager not available for getting saved level {}", levelId);
    return nullptr;
};

void jumpscares::util::clearDownloadDelegate(LevelDownloadDelegate* delegate) {
    if (auto glm = GameLevelManager::sharedState()) {
        if (glm->m_levelDownloadDelegate == delegate) {
            glm->m_levelDownloadDelegate = nullptr;
            log::trace("Cleared download delegate for level");
        };
    };
};

void jumpscares::util::clearLevelManagerDelegate(LevelManagerDelegate* delegate) {
    if (auto glm = GameLevelManager::sharedState()) {
        if (glm->m_levelManagerDelegate == delegate) {
            glm->m_levelManagerDelegate = nullptr;
            log::trace("Cleared level manager delegate for search");
        };
    };
};

constexpr int jumpscares::util::getJumpscareSongId(int levelId) noexcept {
    switch (levelId) {
        case 105001928: return 482872;  // Grief
        case 93437568: return 895761;   // Congregation

        default: return 0;
    };
};

void jumpscares::util::downloadLevelWithDelegate(int levelId, int songId, LevelDownloadDelegate* delegate) {
    log::debug("Initiating download for level {} with song {}", levelId, songId);

    if (auto glm = GameLevelManager::sharedState()) {
        if (auto mdm = MusicDownloadManager::sharedState()) {
            log::debug("Downloading {} in background", levelId);
            glm->downloadLevel(levelId, false, 0);

            if (songId > 0) {
                mdm->downloadSong(songId);
                log::debug("Downloading song {} for level {}", songId, levelId);
            };

            log::debug("Delegate set for level download");
            glm->m_levelDownloadDelegate = delegate;
        } else {
            log::error("MusicDownloadManager not available for level {}", levelId);
        };
    } else {
        log::error("GameLevelManager not available for level {}", levelId);
    };
};

GJSearchObject* jumpscares::util::createLevelSearchObject(int levelId) {
    return GJSearchObject::create(SearchType::Type19, numToString(levelId));
};

void jumpscares::util::switchToLevel(PlayLayer* pl, int levelID, std::string_view levelName, PlayerObject* player, GameObject* killer, bool dontCreateObjects, bool useReplay) {
    log::debug("Attempting to switch to level {} ({})", levelName, levelID);
    auto targetLevel = getSavedDownloadedLevel(levelID);

    if (targetLevel && !targetLevel->m_levelNotDownloaded) {
        if (pl->m_level && pl->m_level->m_levelID.value() == levelID) {
            log::trace("Already in {} level", levelName);
            return;
        };

        log::debug("Destroying player and exiting current level for {}", levelName);
        pl->destroyPlayer(player, killer);
        pl->onExit();

        log::warn("Switching to {} level ({})", levelName, levelID);
        auto scene = PlayLayer::scene(targetLevel, useReplay, dontCreateObjects);
        CCDirector::sharedDirector()->replaceScene(scene);
        return;
    };

    log::debug("{} level {} is not downloaded yet, starting async download", levelName, levelID);

    if (auto glm = GameLevelManager::sharedState()) {
        auto search = createLevelSearchObject(levelID);
        if (!search) {
            log::error("Failed to create search object for {}", levelID);
            return;
        };

        if (auto storedLevels = glm->getStoredOnlineLevels(search->getKey()); storedLevels && storedLevels->count() > 0) {
            if (auto level = typeinfo_cast<GJGameLevel*>(storedLevels->objectAtIndex(0))) {
                log::debug("Using stored level for download of {}", levelName);
                auto delegate = new DownloadDelegate(pl, levelID, std::string(levelName), dontCreateObjects, useReplay);
                util::downloadLevelWithDelegate(level->m_levelID.value(), getJumpscareSongId(levelID), delegate);
                return;
            };
        };

        log::debug("No stored levels found, initiating online search for {}", levelName);
        auto delegate = new SearchDelegate(pl, levelID, std::string(levelName), getJumpscareSongId(levelID), dontCreateObjects, useReplay);
        glm->m_levelManagerDelegate = delegate;
        glm->getOnlineLevels(search);
    } else {
        log::error("GameLevelManager not available for switching to level {}", levelID);
    };
};

void jumpscares::util::download(int levelId, int songId, LevelDownloadDelegate* delegate) {
    log::debug("Download request for level {}", levelId);
    if (auto glm = GameLevelManager::sharedState()) {
        if (glm->hasDownloadedLevel(levelId)) {
            log::info("Level {} already downloaded, skipping download", levelId);
            if (auto savedLevel = getSavedDownloadedLevel(levelId)) glm->updateLevel(savedLevel);
        } else {
            log::debug("Level {} not cached, starting download", levelId);
            util::downloadLevelWithDelegate(levelId, songId, delegate);
        };
    } else {
        log::error("Cannot download {}: GameLevelManager not available", levelId);
    };
};

void jumpscares::downloadGrief(LevelDownloadDelegate* delegate) {
    util::download(129066933, 482872, delegate);
};

void jumpscares::downloadCongregation(LevelDownloadDelegate* delegate) {
    util::download(129066879, 895761, delegate);
};