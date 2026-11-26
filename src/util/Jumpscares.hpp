#pragma once

#include <util/Base.h>

#include <Geode/Geode.hpp>

namespace cs::brkd::horrible {
    namespace util {
        // Jumpscare level manager
        namespace jumpscares {
            using Callback = geode::CopyableFunction<void(geode::Ref<GJGameLevel>)>;

            void switchLevel(int level, bool dontCreateObjects, Callback&& callback = nullptr);
            void downloadLevel(int level, Callback&& callback);
        };
    };
};