#pragma once

#include <Geode/Geode.hpp>

namespace horrible {
    namespace util {
        // Jumpscare level manager
        namespace jumpscares {
            namespace util {
                void switchToLevel(PlayLayer* pl, int levelID, std::string_view levelName, PlayerObject* player, GameObject* killer, bool dontCreateObjects, bool useReplay);

                void download(int levelId, int songId, LevelDownloadDelegate* delegate);
            };

            void downloadGrief(LevelDownloadDelegate* delegate = nullptr);         // Download Grief
            void downloadCongregation(LevelDownloadDelegate* delegate = nullptr);  // Download Congregation
        };
    };
};