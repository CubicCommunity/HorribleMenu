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

    JumpscareDelegateManager::get()->clearDownloadDelegate();
    log::debug("Delegate cleared, checking level validity");

    if (!level || level->m_levelID.value() != getLevelID()) {
        log::error("Downloaded level mismatch or null: expected {}, got {}", getLevelID(), level ? level->m_levelID.value() : -1);
        return;
    };

    if (level->m_levelNotDownloaded) {
        log::error("Downloaded level {} is still not ready", getLevelID());
        return;
    };

    if (getPlayLayer().lock()) {
        log::warn("Downloaded and switching to {} level ({})", getLevelName(), getLevelID());
        auto scene = PlayLayer::scene(level, getUseReplay(), getDontCreateObjects());
        CCDirector::sharedDirector()->replaceScene(scene);
    } else {
        log::warn("PlayLayer weak pointer expired, cannot switch scene for level {}", getLevelName());
    };
};

void jumpscares::DownloadDelegate::levelDownloadFailed(int response) {
    JumpscareDelegateManager::get()->clearDownloadDelegate();
    log::error("Failed to download {} ({})", getLevelName(), response);
};

jumpscares::SearchDelegate::SearchDelegate(PlayLayer* pl, int levelID, int songID, std::string levelName, bool dontCreateObjects, bool useReplay) :
    JumpscareDelegateData(pl, levelID, songID, std::move(levelName), dontCreateObjects, useReplay) {};

void jumpscares::SearchDelegate::loadLevelsFinished(CCArray* levels, char const* key) {
    log::trace("Search finished for level {}", getLevelName());
    JumpscareDelegateManager::get()->clearSearchDelegate();

    if (!levels || levels->count() == 0) {
        log::error("No online level result returned for {}", getLevelName());
        return;
    };

    if (auto level = typeinfo_cast<GJGameLevel*>(levels->objectAtIndex(0))) {
        log::debug("Level object retrieved from search, starting download for {}", getLevelName());
        if (auto pl = getPlayLayer().lock()) {
            auto delegate = std::make_shared<DownloadDelegate>(pl.take(), getLevelID(), getSongID(), getLevelName(), getDontCreateObjects(), getUseReplay());
            downloadLevelAsync(delegate);
        } else {
            log::warn("Search finished but PlayLayer expired for {}", getLevelName());
        };
    } else {
        log::error("Online search returned non-level object for {}", getLevelName());
    };
};

void jumpscares::SearchDelegate::loadLevelsFailed(char const* key) {
    JumpscareDelegateManager::get()->clearSearchDelegate();
    log::error("Failed to fetch online level info for {}", getLevelName());
};

GJGameLevel* jumpscares::getSavedDownloadedLevel(int levelID) {
    if (auto glm = GameLevelManager::sharedState()) {
        if (!glm->hasDownloadedLevel(levelID)) return nullptr;
        return glm->getSavedLevel(levelID);
    };

    log::error("GameLevelManager not available for getting saved level {}", levelID);
    return nullptr;
};

void jumpscares::downloadLevelAsync(std::shared_ptr<DownloadDelegate> delegate) {
    log::debug("Initiating download for level {} with song {}", delegate->getLevelID(), delegate->getSongID());

    if (auto glm = GameLevelManager::sharedState()) {
        if (delegate->getSongID() > 0) {
            if (auto mdm = MusicDownloadManager::sharedState()) {
                mdm->downloadSong(delegate->getSongID());
                log::debug("Downloading song {} for level {}", delegate->getSongID(), delegate->getLevelID());
            } else {
                log::error("MusicDownloadManager not available for level {}", delegate->getLevelID());
            };
        };

        log::debug("Downloading {} in background", delegate->getLevelID());
        glm->downloadLevel(delegate->getLevelID(), false, 0);
        JumpscareDelegateManager::get()->setDownloadDelegate(std::move(delegate));

        log::debug("Delegate set for level download");
    } else {
        log::error("GameLevelManager not available for level {}", delegate->getLevelID());
    };
};

GJSearchObject* jumpscares::createLevelSearchObject(int levelID) {
    return GJSearchObject::create(SearchType::Type19, numToString(levelID));
};

void jumpscares::switchToLevel(PlayLayer* pl, std::shared_ptr<DownloadDelegate> delegate, PlayerObject* player, GameObject* killer, bool dontCreateObjects, bool useReplay) {
    if (!delegate) {
        delegate = JumpscareDelegateManager::get()->getDownloadDelegate().lock();

        if (!delegate) {
            log::error("No active download delegate available to switch to level");
            return;
        };
    };

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

        auto del = std::make_shared<SearchDelegate>(pl, delegate->getLevelID(), 0, delegate->getLevelName(), dontCreateObjects, useReplay);
        glm->getOnlineLevels(search);
        JumpscareDelegateManager::get()->setSearchDelegate(std::move(del));
    } else {
        log::error("GameLevelManager not available for switching to level {}", delegate->getLevelName());
    };
};

std::shared_ptr<jumpscares::DownloadDelegate> jumpscares::get::grief() {
    return std::make_shared<jumpscares::DownloadDelegate>(PlayLayer::get(), 129066933, 482872, "Grief", false, false);
};

std::shared_ptr<jumpscares::DownloadDelegate> jumpscares::get::congregation() {
    return std::make_shared<jumpscares::DownloadDelegate>(PlayLayer::get(), 129066879, 895761, "Congregation", false, false);
};

std::weak_ptr<jumpscares::SearchDelegate> jumpscares::JumpscareDelegateManager::getSearchDelegate() const noexcept {
    return m_searchDelegate;
};

std::weak_ptr<jumpscares::DownloadDelegate> jumpscares::JumpscareDelegateManager::getDownloadDelegate() const noexcept {
    return m_downloadDelegate;
};

void jumpscares::JumpscareDelegateManager::setSearchDelegate(std::shared_ptr<SearchDelegate> delegate) {
    if (auto glm = GameLevelManager::sharedState()) glm->m_levelManagerDelegate = delegate.get();
    m_searchDelegate = std::move(delegate);
};

void jumpscares::JumpscareDelegateManager::setDownloadDelegate(std::shared_ptr<DownloadDelegate> delegate) {
    if (auto glm = GameLevelManager::sharedState()) glm->m_levelDownloadDelegate = delegate.get();
    m_downloadDelegate = std::move(delegate);
};

void jumpscares::JumpscareDelegateManager::clearSearchDelegate() {
    if (auto glm = GameLevelManager::sharedState()) {
        if (m_searchDelegate && glm->m_levelManagerDelegate == m_searchDelegate.get()) {
            glm->m_levelManagerDelegate = nullptr;

            log::trace("Cleared level manager delegate for search");
        } else if (m_searchDelegate) {
            log::warn("Level manager delegate for search {} was not set or already cleared", m_searchDelegate->getLevelID());
        } else {
            log::warn("No search delegate to clear");
        };

        m_searchDelegate.reset();
    };
};

void jumpscares::JumpscareDelegateManager::clearDownloadDelegate() {
    if (auto glm = GameLevelManager::sharedState()) {
        if (m_downloadDelegate && glm->m_levelDownloadDelegate == m_downloadDelegate.get()) {
            glm->m_levelDownloadDelegate = nullptr;

            log::trace("Cleared download delegate for level");
        } else if (m_downloadDelegate) {
            log::warn("Download delegate for level {} was not set or already cleared", m_downloadDelegate->getLevelID());
        } else {
            log::warn("No download delegate to clear");
        };

        m_downloadDelegate.reset();
    };
};

jumpscares::JumpscareDelegateManager* jumpscares::JumpscareDelegateManager::get() noexcept {
    static auto inst = new (std::nothrow) JumpscareDelegateManager();
    return inst;
};