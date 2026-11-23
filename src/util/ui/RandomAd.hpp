#pragma once

#include <Geode/Geode.hpp>

namespace cs::brkd::horrible {
    namespace ui {
        class RandomAd final : public geode::Popup {
        protected:
            bool init();

        public:
            static RandomAd* create();
        };
    };
};