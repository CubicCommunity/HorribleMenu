#pragma once

#include <Geode/Geode.hpp>

namespace horrible {
    namespace util {
        // Jumpscare level manager
        namespace jumpscares {
            namespace util {
                struct SearchDelegate final : public LevelManagerDelegate {
                private:
                    geode::WeakRef<PlayLayer> m_playLayer;

                    int m_levelId;
                    std::string m_levelName;

                    int m_songId;
                    bool m_dontCreateObjects;
                    bool m_useReplay;

                public:
                    SearchDelegate(PlayLayer* pl, int levelId, std::string levelName, int songId, bool dontCreateObjects, bool useReplay);

                    void loadLevelsFinished(cocos2d::CCArray* levels, char const* key) override;
                    void loadLevelsFailed(char const* key) override;
                };

                struct DownloadDelegate final : public LevelDownloadDelegate {
                private:
                    geode::WeakRef<PlayLayer> m_playLayer;

                    int m_levelId;
                    std::string m_levelName;

                    bool m_dontCreateObjects;
                    bool m_useReplay;

                public:
                    DownloadDelegate(PlayLayer* pl, int levelId, std::string levelName, bool dontCreateObjects, bool useReplay);

                    void levelDownloadFinished(GJGameLevel* level) override;
                    void levelDownloadFailed(int response) override;
                };

                void switchToLevel(PlayLayer* pl, int levelID, std::string_view levelName, PlayerObject* player, GameObject* killer, bool dontCreateObjects, bool useReplay);

                void download(int levelId, int songId, LevelDownloadDelegate* delegate);

                GJGameLevel* getSavedDownloadedLevel(int levelId);
                GJSearchObject* createLevelSearchObject(int levelId);

                void downloadLevelWithDelegate(int levelId, int songId, LevelDownloadDelegate* delegate);
                void clearDownloadDelegate(LevelDownloadDelegate* delegate);
                void clearLevelManagerDelegate(LevelManagerDelegate* delegate);

                constexpr int getJumpscareSongId(int levelId) noexcept;
            };

            void downloadGrief(LevelDownloadDelegate* delegate = nullptr);         // Download Grief
            void downloadCongregation(LevelDownloadDelegate* delegate = nullptr);  // Download Congregation
        };
    };
};