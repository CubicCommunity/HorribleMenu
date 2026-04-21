#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace horrible {
    namespace ui {
        class RandomAd final : public Popup, public LevelDownloadDelegate {
        protected:
            bool init();

        public:
            static RandomAd* create();
        };
    };
};