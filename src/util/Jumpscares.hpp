#pragma once

#include <Geode/Geode.hpp>

namespace horrible {
    namespace util {
        // Jumpscare level manager
        namespace jumpscares {
            struct LevelInfo final {
                int id;
                int songID;
            };

            namespace level {
                inline constexpr LevelInfo grief = {
                    129066933,
                    482872,
                };

                inline constexpr LevelInfo congregation = {
                    129066879,
                    895761,
                };
            };

            void switchLevel(LevelInfo const& level, bool dontCreateObjects, bool useReplay);

            namespace coro {
                void getLevel(int id, geode::CopyableFunction<void(geode::Result<GJGameLevel*>)>&& callback);
            };

            class JumpscareLevelManager final : public MusicDownloadDelegate {
            private:
                std::unordered_map<int, geode::Ref<GJGameLevel>> m_levels;

            protected:
                JumpscareLevelManager() = default;
                ~JumpscareLevelManager() = default;

                JumpscareLevelManager(const JumpscareLevelManager&) = delete;
                JumpscareLevelManager& operator=(const JumpscareLevelManager&) = delete;

                JumpscareLevelManager(JumpscareLevelManager&&) = delete;
                JumpscareLevelManager& operator=(JumpscareLevelManager&&) = delete;

            public:
                static JumpscareLevelManager* get() noexcept;

                void saveLevel(GJGameLevel* level);

                GJGameLevel* getLevel(int id) const noexcept;

                // mainly for debug

                void loadSongInfoFinished(SongInfoObject* object) override;
                void loadSongInfoFailed(int id, GJSongError errorType) override;
                void downloadSongStarted(int id) override;
                void downloadSongFinished(int id) override;
            };
        };
    };
};