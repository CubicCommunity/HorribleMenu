#pragma once

#include <Geode/Geode.hpp>

namespace horrible {
    namespace ui {
        class TermsAndConditions final : public geode::Popup {
            using Callback = geode::Function<void(bool)>;

        private:
            struct Impl;
            std::unique_ptr<Impl> m_impl;

        protected:
            TermsAndConditions();
            ~TermsAndConditions();

            void finishBtnFade(cocos2d::CCNode* sender);
            void update(float dt) override;

            bool init(Callback&& cb);

        public:
            static TermsAndConditions* create(Callback&& cb);
        };
    };
};