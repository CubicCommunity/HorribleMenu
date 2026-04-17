#include "../Jumpscares.hpp"

#include <Utils.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::util;

jumpscares::util::JumpscareDownloadDelegate::JumpscareDownloadDelegate(PlayLayer* pl, int levelId, std::string levelName, bool dontCreateObjects, bool useReplay) :
    m_playLayer(pl), m_levelId(levelId), m_levelName(std::move(levelName)), m_dontCreateObjects(dontCreateObjects), m_useReplay(useReplay) {}

void jumpscares::util::JumpscareDownloadDelegate::levelDownloadFinished(GJGameLevel* level) {
    if (!level || level->m_levelID.value() != m_levelId) {
        clearDownloadDelegate(this);
        delete this;
        return;
    };

    clearDownloadDelegate(this);

    if (level->m_levelNotDownloaded) {
        log::error("Downloaded level {} is still not ready", m_levelId);
        delete this;
        return;
    };

    if (auto playLayer = m_playLayer.lock()) {
        auto scene = PlayLayer::scene(level, m_useReplay, m_dontCreateObjects);
        CCDirector::get()->replaceScene(scene);
        log::warn("Downloaded and switching to {} level ({})", m_levelName, m_levelId);
    };

    delete this;
};

void jumpscares::util::JumpscareDownloadDelegate::levelDownloadFailed(int response) {
    clearDownloadDelegate(this);
    log::error("Failed to download {} ({})", m_levelName, response);
    delete this;
};

jumpscares::util::JumpscareSearchDelegate::JumpscareSearchDelegate(PlayLayer* pl, int levelId, std::string levelName, int songId, bool dontCreateObjects, bool useReplay) :
    m_playLayer(pl), m_levelId(levelId), m_levelName(std::move(levelName)), m_songId(songId), m_dontCreateObjects(dontCreateObjects), m_useReplay(useReplay) {};

void jumpscares::util::JumpscareSearchDelegate::loadLevelsFinished(cocos2d::CCArray* levels, char const* key) {
    util::clearLevelManagerDelegate(this);

    if (!levels || levels->count() == 0) {
        log::error("No online level result returned for {}", m_levelName);
        delete this;
        return;
    };

    if (auto level = typeinfo_cast<GJGameLevel*>(levels->objectAtIndex(0))) {
        auto delegate = new jumpscares::util::JumpscareDownloadDelegate(m_playLayer.lock(), m_levelId, m_levelName, m_dontCreateObjects, m_useReplay);
        util::downloadLevelWithDelegate(level->m_levelID.value(), m_songId, delegate);
    } else {
        log::error("Online search returned non-level object for {}", m_levelName);
        delete this;
    };
};

void jumpscares::util::JumpscareSearchDelegate::loadLevelsFailed(char const* key) {
    util::clearLevelManagerDelegate(this);
    log::error("Failed to fetch online level info for {}", m_levelName);
    delete this;
};

GJGameLevel* jumpscares::util::getSavedDownloadedLevel(int levelId) {
    if (auto glm = GameLevelManager::sharedState()) {
        if (!glm->hasDownloadedLevel(levelId)) return nullptr;
        return glm->getSavedLevel(levelId);
    };

    return nullptr;
};

void jumpscares::util::clearDownloadDelegate(LevelDownloadDelegate* delegate) {
    if (auto glm = GameLevelManager::sharedState()) {
        if (glm->m_levelDownloadDelegate == delegate) glm->m_levelDownloadDelegate = nullptr;
    };
};

void jumpscares::util::clearLevelManagerDelegate(LevelManagerDelegate* delegate) {
    if (auto glm = GameLevelManager::sharedState()) {
        if (glm->m_levelManagerDelegate == delegate) glm->m_levelManagerDelegate = nullptr;
    };
};

int jumpscares::util::getJumpscareSongId(int levelId) {
    switch (levelId) {
        case 105001928: return 482872;  // Grief
        case 93437568: return 895761;   // Congregation

        default: return 0;
    };
};

void jumpscares::util::downloadLevelWithDelegate(int levelId, int songId, LevelDownloadDelegate* delegate) {
    if (auto glm = GameLevelManager::sharedState()) {
        if (auto mdm = MusicDownloadManager::sharedState()) {
            log::debug("Downloading {} in background", levelId);
            glm->downloadLevel(levelId, false, 0);

            if (songId > 0) {
                mdm->downloadSong(songId);
                log::debug("Downloading song {} for level {}", songId, levelId);
            };

            glm->m_levelDownloadDelegate = delegate;
        };
    };
};

GJSearchObject* jumpscares::util::createLevelSearchObject(int levelId) {
    return GJSearchObject::create(SearchType::Type19, numToString(levelId));
};

void jumpscares::util::switchToLevel(PlayLayer* pl, int levelID, std::string_view levelName, PlayerObject* player, GameObject* killer, bool dontCreateObjects, bool useReplay) {
    auto targetLevel = getSavedDownloadedLevel(levelID);

    if (targetLevel && !targetLevel->m_levelNotDownloaded) {
        if (pl->m_level && pl->m_level->m_levelID.value() == levelID) {
            log::trace("Already in {} level", levelName);
            return;
        };

        pl->destroyPlayer(player, killer);
        pl->onExit();

        auto scene = PlayLayer::scene(targetLevel, useReplay, dontCreateObjects);
        CCDirector::get()->replaceScene(scene);

        log::warn("Switching to {} level ({})", levelName, levelID);
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
                auto delegate = new JumpscareDownloadDelegate(pl, levelID, std::string(levelName), dontCreateObjects, useReplay);
                util::downloadLevelWithDelegate(level->m_levelID.value(), getJumpscareSongId(levelID), delegate);
                return;
            };
        };

        auto delegate = new JumpscareSearchDelegate(pl, levelID, std::string(levelName), getJumpscareSongId(levelID), dontCreateObjects, useReplay);
        glm->m_levelManagerDelegate = delegate;
        glm->getOnlineLevels(search);
    };
};

void jumpscares::util::download(int levelId, int songId, LevelDownloadDelegate* delegate) {
    if (auto glm = GameLevelManager::sharedState()) {
        if (glm->hasDownloadedLevel(levelId)) {
            log::info("{} already downloaded, skipping download", levelId);
            if (auto savedLevel = getSavedDownloadedLevel(levelId)) glm->updateLevel(savedLevel);
        } else {
            util::downloadLevelWithDelegate(levelId, songId, delegate);
        };
    } else {
        log::error("Cannot download {}", levelId);
    };
};

void jumpscares::downloadGrief(LevelDownloadDelegate* delegate) {
    util::download(129066933, 482872, delegate);
};

void jumpscares::downloadCongregation(LevelDownloadDelegate* delegate) {
    util::download(129066879, 895761, delegate);
};