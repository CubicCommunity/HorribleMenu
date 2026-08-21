#pragma once

#include <Geode/Geode.hpp>

namespace horrible {
    namespace ui {
        class JumpHealthMeter final : public cocos2d::CCNode {
            using Callback = geode::Function<void()>;

        private:
            struct Impl;
            std::unique_ptr<Impl> m_impl;

        protected:
            JumpHealthMeter();
            ~JumpHealthMeter();

            void update(float dt) override;

            bool init(Callback&& cb);

        public:
            static JumpHealthMeter* create(Callback&& cb);

            void jump();
            void reset();

            void playPause(bool play);
        };
    };
};