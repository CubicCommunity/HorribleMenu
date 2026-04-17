#include "../Jumpscares.hpp"

#include <Utils.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;

using namespace horrible::util;

void jumpscares::util::switchToLevel(PlayLayer* pl, int levelID, std::string_view levelName, PlayerObject* player, GameObject* killer, bool dontCreateObjects, bool useReplay) {
    auto glm = GameLevelManager::get();
    auto targetLevel = glm->getSavedLevel(levelID);

    if (!targetLevel || targetLevel->m_levelNotDownloaded) return;

    if (pl->m_level && pl->m_level->m_levelID.value() == levelID) {
        log::trace("Already in {} level", levelName);
        return;
    };

    pl->destroyPlayer(player, killer);
    pl->onExit();

    auto scene = PlayLayer::scene(targetLevel, useReplay, dontCreateObjects);
    CCDirector::get()->replaceScene(scene);

    log::warn("Switching to {} level ({})", levelName, levelID);
};

void jumpscares::util::download(int levelId, int songId, LevelDownloadDelegate* delegate) {
    if (auto glm = GameLevelManager::sharedState()) {
        if (glm->hasDownloadedLevel(levelId)) {
            log::info("{} already downloaded, skipping download", levelId);
            glm->updateLevel(glm->getSavedLevel(levelId));
        } else if (auto mdm = MusicDownloadManager::sharedState()) {
            log::debug("Downloading {} in background", levelId);

            glm->downloadLevel(levelId, false, 0);
            mdm->downloadSong(songId);

            glm->m_levelDownloadDelegate = delegate;
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