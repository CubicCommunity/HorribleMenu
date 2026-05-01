#include "../Jumpscares.hpp"

#include <Utils.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::util;

jumpscares::JumpscareDelegateData::JumpscareDelegateData(PlayLayer* pl, int levelID, int songID, std::string levelName, bool dontCreateObjects, bool useReplay) :
    m_playLayer(pl), m_levelID(levelID), m_songID(songID), m_levelName(std::move(levelName)), m_dontCreateObjects(dontCreateObjects), m_useReplay(useReplay) {};

WeakRef<PlayLayer> const& jumpscares::JumpscareDelegateData::getPlayLayer() const noexcept {
    return m_playLayer;
};

int jumpscares::JumpscareDelegateData::getLevelID() const noexcept {
    return m_levelID;
};

int jumpscares::JumpscareDelegateData::getSongID() const noexcept {
    return m_songID;
};

ZStringView jumpscares::JumpscareDelegateData::getLevelName() const noexcept {
    return m_levelName;
};

bool jumpscares::JumpscareDelegateData::getDontCreateObjects() const noexcept {
    return m_dontCreateObjects;
};

bool jumpscares::JumpscareDelegateData::getUseReplay() const noexcept {
    return m_useReplay;
};

jumpscares::DownloadDelegate::DownloadDelegate(PlayLayer* pl, int levelID, int songID, std::string levelName, bool dontCreateObjects, bool useReplay) :
    JumpscareDelegateData(pl, levelID, songID, std::move(levelName), dontCreateObjects, useReplay) {};

void jumpscares::DownloadDelegate::levelDownloadFinished(GJGameLevel* level) {
    log::trace("Download finished for level {}", getLevelID());

    if (!level || level->m_levelID.value() != getLevelID()) {
        log::error("Downloaded level mismatch or null: expected {}, got {}", getLevelID(), level ? level->m_levelID.value() : -1);
        clearDownloadDelegate(this);
        delete this;
        return;
    };

    log::debug("Delegate cleared, checking level validity");
    clearDownloadDelegate(this);

    if (level->m_levelNotDownloaded) {
        log::error("Downloaded level {} is still not ready", getLevelID());
        delete this;
        return;
    };

    if (getPlayLayer().lock()) {
        log::warn("Downloaded and switching to {} level ({})", getLevelName(), getLevelID());
        auto scene = PlayLayer::scene(level, getUseReplay(), getDontCreateObjects());
        CCDirector::sharedDirector()->replaceScene(scene);
    } else {
        log::warn("PlayLayer weak pointer expired, cannot switch scene for level {}", getLevelName());
    };

    delete this;
};

void jumpscares::DownloadDelegate::levelDownloadFailed(int response) {
    clearDownloadDelegate(this);
    log::error("Failed to download {} ({})", getLevelName(), response);
    delete this;
};

jumpscares::SearchDelegate::SearchDelegate(PlayLayer* pl, int levelID, int songID, std::string levelName, bool dontCreateObjects, bool useReplay) :
    JumpscareDelegateData(pl, levelID, songID, std::move(levelName), dontCreateObjects, useReplay) {};

void jumpscares::SearchDelegate::loadLevelsFinished(CCArray* levels, char const* key) {
    log::trace("Search finished for level {}", getLevelName());
    clearLevelManagerDelegate(this);

    if (!levels || levels->count() == 0) {
        log::error("No online level result returned for {}", getLevelName());
        delete this;
        return;
    };

    if (auto level = typeinfo_cast<GJGameLevel*>(levels->objectAtIndex(0))) {
        log::debug("Level object retrieved from search, starting download for {}", getLevelName());
        auto delegate = new DownloadDelegate(getPlayLayer().lock().take(), getLevelID(), getSongID(), getLevelName(), getDontCreateObjects(), getUseReplay());
        downloadLevelAsync(delegate);
    } else {
        log::error("Online search returned non-level object for {}", getLevelName());
        delete this;
    };
};

void jumpscares::SearchDelegate::loadLevelsFailed(char const* key) {
    clearLevelManagerDelegate(this);
    log::error("Failed to fetch online level info for {}", getLevelName());
    delete this;
};

GJGameLevel* jumpscares::getSavedDownloadedLevel(int levelID) {
    if (auto glm = GameLevelManager::sharedState()) {
        if (!glm->hasDownloadedLevel(levelID)) return nullptr;
        return glm->getSavedLevel(levelID);
    };

    log::error("GameLevelManager not available for getting saved level {}", levelID);
    return nullptr;
};

void jumpscares::clearDownloadDelegate(DownloadDelegate* delegate) {
    if (auto glm = GameLevelManager::sharedState()) {
        if (glm->m_levelDownloadDelegate == delegate) {
            glm->m_levelDownloadDelegate = nullptr;
            log::trace("Cleared download delegate for level");
        } else {
            log::warn("Download delegate for level {} was not set or already cleared", delegate->getLevelID());
        };
    };
};

void jumpscares::clearLevelManagerDelegate(SearchDelegate* delegate) {
    if (auto glm = GameLevelManager::sharedState()) {
        if (glm->m_levelManagerDelegate == delegate) {
            glm->m_levelManagerDelegate = nullptr;
            log::trace("Cleared level manager delegate for search");
        } else {
            log::warn("Level manager delegate for search {} was not set or already cleared", delegate->getLevelID());
        };
    };
};

void jumpscares::saveLevel(DownloadDelegate* delegate) {
    log::debug("Download request for level {}", delegate->getLevelID());

    if (auto glm = GameLevelManager::sharedState()) {
        if (glm->hasDownloadedLevel(delegate->getLevelID())) {
            log::info("Level {} already downloaded, skipping download", delegate->getLevelID());
            if (auto savedLevel = getSavedDownloadedLevel(delegate->getLevelID())) glm->updateLevel(savedLevel);

            delete delegate;
        } else {
            log::debug("Level {} not cached, starting download", delegate->getLevelID());
            downloadLevelAsync(delegate);
        };
    } else {
        log::error("Cannot download {}: GameLevelManager not available", delegate->getLevelID());
        delete delegate;
    };
};

void jumpscares::downloadLevelAsync(DownloadDelegate* delegate) {
    log::debug("Initiating download for level {} with song {}", delegate->getLevelID(), delegate->getSongID());

    if (auto glm = GameLevelManager::sharedState()) {
        log::debug("Downloading {} in background", delegate->getLevelID());
        glm->downloadLevel(delegate->getLevelID(), false, 0);

        if (delegate->getSongID() > 0) {
            if (auto mdm = MusicDownloadManager::sharedState()) {
                mdm->downloadSong(delegate->getSongID());
                log::debug("Downloading song {} for level {}", delegate->getSongID(), delegate->getLevelID());
            } else {
                log::error("MusicDownloadManager not available for level {}", delegate->getLevelID());
            };
        };

        log::debug("Delegate set for level download");
        glm->m_levelDownloadDelegate = delegate;
    } else {
        log::error("GameLevelManager not available for level {}", delegate->getLevelID());
    };
};

GJSearchObject* jumpscares::createLevelSearchObject(int levelID) {
    return GJSearchObject::create(SearchType::Type19, numToString(levelID));
};

void jumpscares::switchToLevel(PlayLayer* pl, DownloadDelegate* delegate, PlayerObject* player, GameObject* killer, bool dontCreateObjects, bool useReplay) {
    log::debug("Attempting to switch to level {} ({})", delegate->getLevelName(), delegate->getLevelID());

    if (auto targetLevel = getSavedDownloadedLevel(delegate->getLevelID())) {
        if (pl->m_level && pl->m_level->m_levelID.value() == delegate->getLevelID()) {
            log::trace("Already in {} level", delegate->getLevelName());
            return;
        };

        log::debug("Destroying player and exiting current level for {}", delegate->getLevelName());

        pl->destroyPlayer(player, killer);
        pl->onExit();

        if (!targetLevel->m_levelNotDownloaded) {
            log::warn("Switching to {} level ({})", delegate->getLevelName(), delegate->getLevelID());

            auto scene = PlayLayer::scene(targetLevel, useReplay, dontCreateObjects);
            CCDirector::sharedDirector()->replaceScene(scene);
        } else {
            log::warn("{} level {} is not ready yet, starting async download", delegate->getLevelName(), delegate->getLevelID());
            downloadLevelAsync(delegate);
        };

        return;
    };

    log::debug("{} level {} is not downloaded yet, starting async download", delegate->getLevelName(), delegate->getLevelID());

    if (auto glm = GameLevelManager::sharedState()) {
        auto search = createLevelSearchObject(delegate->getLevelID());
        if (!search) {
            log::error("Failed to create search object for {}", delegate->getLevelID());
            return;
        };

        if (auto storedLevels = glm->getStoredOnlineLevels(search->getKey()); storedLevels && storedLevels->count() > 0) {
            if (auto level = typeinfo_cast<GJGameLevel*>(storedLevels->objectAtIndex(0))) {
                log::debug("Using stored level for download of {}", delegate->getLevelName());
                downloadLevelAsync(delegate);
                return;
            } else {
                log::error("Stored level search result was not a level for {}", delegate->getLevelName());
            };
        } else {
            log::warn("No stored levels found for {}", delegate->getLevelName());
        };

        log::debug("No stored levels found, initiating online search for {}", delegate->getLevelName());

        auto del = new SearchDelegate(pl, delegate->getLevelID(), 0, delegate->getLevelName(), dontCreateObjects, useReplay);
        glm->m_levelManagerDelegate = del;
        glm->getOnlineLevels(search);
    } else {
        log::error("GameLevelManager not available for switching to level {}", delegate->getLevelName());
    };
};

jumpscares::DownloadDelegate* jumpscares::get::grief() {
    return new jumpscares::DownloadDelegate(PlayLayer::get(), 129066933, 482872, "Grief", false, false);
};

jumpscares::DownloadDelegate* jumpscares::get::congregation() {
    return new jumpscares::DownloadDelegate(PlayLayer::get(), 129066879, 895761, "Congregation", false, false);
};