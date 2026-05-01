#pragma once

#include <Geode/Geode.hpp>

namespace horrible {
    namespace util {
        // Jumpscare level manager
        namespace jumpscares {

            class JumpscareDelegateData {
            private:
                geode::WeakRef<PlayLayer> m_playLayer = nullptr;

                int m_levelID = 0;
                int m_songID = 0;
                std::string m_levelName = "";

                bool m_dontCreateObjects = false;
                bool m_useReplay = false;

            protected:
                explicit JumpscareDelegateData(PlayLayer* pl, int levelID, int songID, std::string levelName, bool dontCreateObjects, bool useReplay);
                virtual ~JumpscareDelegateData() = default;

            public:
                geode::WeakRef<PlayLayer> const& getPlayLayer() const noexcept;

                int getLevelID() const noexcept;
                int getSongID() const noexcept;
                geode::ZStringView getLevelName() const noexcept;

                bool getDontCreateObjects() const noexcept;
                bool getUseReplay() const noexcept;
            };

            struct SearchDelegate final : public LevelManagerDelegate, public JumpscareDelegateData, std::enable_shared_from_this<SearchDelegate> {
            public:
                SearchDelegate(PlayLayer* pl, int levelID, int songID, std::string levelName, bool dontCreateObjects, bool useReplay);

                void loadLevelsFinished(cocos2d::CCArray* levels, char const* key) override;
                void loadLevelsFailed(char const* key) override;
            };

            struct DownloadDelegate final : public LevelDownloadDelegate, public JumpscareDelegateData, std::enable_shared_from_this<DownloadDelegate> {
            public:
                DownloadDelegate(PlayLayer* pl, int levelID, int songID, std::string levelName, bool dontCreateObjects, bool useReplay);

                void levelDownloadFinished(GJGameLevel* level) override;
                void levelDownloadFailed(int response) override;
            };

            namespace get {
                std::shared_ptr<DownloadDelegate> grief();
                std::shared_ptr<DownloadDelegate> congregation();
            };

            void switchToLevel(PlayLayer* pl, std::shared_ptr<DownloadDelegate> delegate, PlayerObject* player, GameObject* killer, bool dontCreateObjects, bool useReplay);

            void downloadLevelAsync(std::shared_ptr<DownloadDelegate> delegate);

            GJGameLevel* getSavedDownloadedLevel(int levelID);
            GJSearchObject* createLevelSearchObject(int levelID);

            class JumpscareDelegateManager final {
            private:
                std::shared_ptr<SearchDelegate> m_searchDelegate;
                std::shared_ptr<DownloadDelegate> m_downloadDelegate;

            protected:
                JumpscareDelegateManager() = default;
                ~JumpscareDelegateManager() = default;

                JumpscareDelegateManager(const JumpscareDelegateManager&) = delete;
                JumpscareDelegateManager& operator=(const JumpscareDelegateManager&) = delete;

                JumpscareDelegateManager(JumpscareDelegateManager&&) = delete;
                JumpscareDelegateManager& operator=(JumpscareDelegateManager&&) = delete;

            public:
                static JumpscareDelegateManager* get() noexcept;

                std::weak_ptr<SearchDelegate> getSearchDelegate() const noexcept;
                std::weak_ptr<DownloadDelegate> getDownloadDelegate() const noexcept;

                void setSearchDelegate(std::shared_ptr<SearchDelegate> delegate);
                void setDownloadDelegate(std::shared_ptr<DownloadDelegate> delegate);

                void clearSearchDelegate();
                void clearDownloadDelegate();
            };
        };
    };
};