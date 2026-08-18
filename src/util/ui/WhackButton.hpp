#pragma once

#include <Geode/Geode.hpp>

namespace horrible {
    namespace ui {
        class WhackButton final : public cocos2d::CCNode {
            using Callback = geode::Function<void(bool)>;

        private:
            struct Impl;
            std::unique_ptr<Impl> m_impl;

            void reload();

        protected:
            WhackButton();
            ~WhackButton();

            void callAfterFeedback(float);
            void setSuccess(bool v);

            void update(float dt) override;

            bool init() override;

        public:
            static WhackButton* create();

            void setCallback(Callback&& cb);
        };
    };
};