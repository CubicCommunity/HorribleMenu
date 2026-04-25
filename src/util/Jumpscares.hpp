#pragma once

#include <Geode/Geode.hpp>

namespace horrible {
    namespace util {
        // Jumpscare level manager
        namespace jumpscares {
            class JumpscareDelegateBase {
            private:
                geode::WeakRef<PlayLayer> m_playLayer = nullptr;

                int m_levelID = 0;
                int m_songID = 0;
                std::string m_levelName = "";

                bool m_dontCreateObjects = false;
                bool m_useReplay = false;

            protected:
                explicit JumpscareDelegateBase(PlayLayer* pl, int levelID, int songID, std::string levelName, bool dontCreateObjects, bool useReplay);
                virtual ~JumpscareDelegateBase() = default;

            public:
                geode::WeakRef<PlayLayer> const& getPlayLayer() const noexcept;

                int getLevelID() const noexcept;
                int getSongID() const noexcept;
                geode::ZStringView getLevelName() const noexcept;

                bool getDontCreateObjects() const noexcept;
                bool getUseReplay() const noexcept;
            };

            struct SearchDelegate final : public LevelManagerDelegate, public JumpscareDelegateBase {
            public:
                SearchDelegate(PlayLayer* pl, int levelID, int songID, std::string levelName, bool dontCreateObjects, bool useReplay);

                void loadLevelsFinished(cocos2d::CCArray* levels, char const* key) override;
                void loadLevelsFailed(char const* key) override;
            };

            struct DownloadDelegate final : public LevelDownloadDelegate, public JumpscareDelegateBase {
            public:
                DownloadDelegate(PlayLayer* pl, int levelID, int songID, std::string levelName, bool dontCreateObjects, bool useReplay);

                void levelDownloadFinished(GJGameLevel* level) override;
                void levelDownloadFailed(int response) override;
            };

            namespace get {
                DownloadDelegate* grief();
                DownloadDelegate* congregation();
            };

            void switchToLevel(PlayLayer* pl, DownloadDelegate* delegate, PlayerObject* player, GameObject* killer, bool dontCreateObjects, bool useReplay);

            void saveLevel(DownloadDelegate* delegate);
            void downloadLevelAsync(DownloadDelegate* delegate);

            GJGameLevel* getSavedDownloadedLevel(int levelID);
            GJSearchObject* createLevelSearchObject(int levelID);

            void clearDownloadDelegate(DownloadDelegate* delegate);
            void clearLevelManagerDelegate(SearchDelegate* delegate);
        };
    };
};