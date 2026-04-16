#include "../Jumpscares.hpp"

#include <Utils.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;

using namespace horrible::util;

void jumpscares::util::download(int levelId, int songId, LevelDownloadDelegate* delegate) {
    if (auto glm = GameLevelManager::sharedState()) {
        if (glm->hasDownloadedLevel(levelId)) {
            log::info("{} already downloaded, skipping download", levelId);
            glm->updateLevel(glm->getSavedLevel(levelId));
        } else if (auto mdm = MusicDownloadManager::sharedState()) {
            log::debug("Downloading {} in background", levelId);

            glm->m_levelDownloadDelegate = delegate;

            glm->downloadLevel(levelId, false, 0);
            mdm->downloadSong(songId);
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