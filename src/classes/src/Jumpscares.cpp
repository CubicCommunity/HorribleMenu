#include "../Jumpscares.hpp"

#include <Utils.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::util;

void jumpscares::downloadGrief() {
    if (auto glm = GameLevelManager::get()) {
        if (glm->hasDownloadedLevel(129066933)) {
            log::info("Grief already downloaded, skipping download");
            glm->updateLevel(glm->getSavedLevel(129066933));
        } else {
            auto mdm = MusicDownloadManager::sharedState();

            log::info("Downloading Grief in background");

            glm->downloadLevel(129066933, false, 0);
            mdm->downloadSong(482872);
        };
    } else {
        log::error("Cannot download Grief");
    };
};

void jumpscares::downloadCongregation() {
    if (auto glm = GameLevelManager::get()) {
        if (glm->hasDownloadedLevel(129066879)) {
            log::info("Congregation already downloaded, skipping download");
            glm->updateLevel(glm->getSavedLevel(129066879));
        } else {
            auto mdm = MusicDownloadManager::sharedState();

            log::info("Downloading Congregation in background");

            glm->downloadLevel(129066879, false, 0);
            mdm->downloadSong(895761);
        };
    } else {
        log::error("Cannot download Congregation");
    };
};