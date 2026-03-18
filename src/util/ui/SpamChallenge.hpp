#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace horrible {
    namespace ui {
        class SpamChallenge final : public CCBlockLayer, public FLAlertLayerProtocol {
        private:
            class Impl;
            std::unique_ptr<Impl> m_impl;

            using Callback = Function<void(bool)>;

        protected:
            SpamChallenge();
            ~SpamChallenge();

            void closeAfterFeedback(float);
            void setSuccess(bool v);

            void keyBackClicked() override;
            void update(float dt) override;

            bool init() override;

        public:
            static SpamChallenge* create();

            bool ccTouchBegan(CCTouch* touch, CCEvent* event) override;

            void setCallback(Callback&& cb);
        };
    };
};