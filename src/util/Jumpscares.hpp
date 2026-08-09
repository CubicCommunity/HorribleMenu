#pragma once

#include <util/Base.h>

#include <Geode/Geode.hpp>

namespace horrible {
    namespace util {
        // Jumpscare level manager
        namespace jumpscares {
            void switchLevel(int level, bool dontCreateObjects, bool useReplay, geode::CopyableFunction<void()>&& callback = nullptr);

            namespace coro {
                void getLevel(int id, geode::CopyableFunction<void(geode::Result<GJGameLevel*>)>&& callback);
            };

            class JumpscareLevelManager final : public base::Singleton<JumpscareLevelManager>, MusicDownloadDelegate {
            private:
                std::unordered_map<int, geode::Ref<GJGameLevel>> m_levels;

            public:
                void saveLevel(GJGameLevel* level);

                GJGameLevel* getLevel(int id) const noexcept;
            };
        };
    };
};