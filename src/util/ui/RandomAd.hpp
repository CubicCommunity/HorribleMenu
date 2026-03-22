#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace horrible {
    namespace ui {  // Popup that displays a random level thumbnail
        class RandomAd final : public Popup {
        protected:
            void onPlayBtn(CCObject*);

            bool init();

        public:
            static RandomAd* create();
        };
    };
};