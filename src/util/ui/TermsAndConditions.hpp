#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/Button.hpp>

namespace horrible {
    namespace ui {
        class TermsAndConditions final : public geode::Popup {
            using Callback = geode::CopyableFunction<void(bool)>;

        private:
            geode::Button* m_acceptBtn = nullptr;
            cocos2d::CCPoint m_acceptVelocity = {0.f, 0.f};
            float m_acceptSpeed = 100.f;
            float m_mouseAvoidDistance = 25.f;
            float m_mouseAvoidMultiplier = 1.75f;

        protected:
            void finishBtnFade(cocos2d::CCNode* sender);
            void update(float dt) override;

            bool init(Callback&& cb);

        public:
            static TermsAndConditions* create(Callback&& cb);
        };
    };
};