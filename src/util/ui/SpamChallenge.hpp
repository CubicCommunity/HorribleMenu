#pragma once

#include <Geode/Geode.hpp>

namespace horrible {
    namespace ui {
        class SpamChallenge final : public CCBlockLayer {
            using Callback = geode::Function<void(bool)>;

        private:
            class Impl;
            std::unique_ptr<Impl> m_impl;

        protected:
            SpamChallenge();
            ~SpamChallenge();

            void callAfterFeedback(float);
            void setSuccess(bool v);

            void update(float dt) override;

            bool init() override;

        public:
            static SpamChallenge* create();

            bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;

            void setCallback(Callback&& cb);
        };
    };
};