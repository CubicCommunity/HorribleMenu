#pragma once

#include <Geode/Geode.hpp>

namespace horrible {
    namespace ui {
        class TermsAndConditions final : public geode::Popup {
            using Callback = geode::CopyableFunction<void(bool)>;

        protected:
            void finishBtnFade(cocos2d::CCNode* sender);

            bool init(Callback&& cb);

        public:
            static TermsAndConditions* create(Callback&& cb);
        };
    };
};