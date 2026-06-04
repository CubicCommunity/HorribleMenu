#pragma once

#include <Geode/Geode.hpp>

namespace horrible {
    namespace util {
        // Jumpscare level manager
        namespace jumpscares {
            namespace level {
                inline constexpr auto grief = 129066933;
                inline constexpr auto congregation = 129066879;
                inline constexpr auto tidal = 93733469;
            };

            void switchLevel(int level, bool dontCreateObjects, bool useReplay, geode::CopyableFunction<void()>&& callback = nullptr);

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
            };
        };
    };
};