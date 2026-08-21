#pragma once

#include <Geode/Geode.hpp>

namespace horrible {
    namespace ui {
        class InputMeter final : public cocos2d::CCNode {
        private:
            struct Impl;
            std::unique_ptr<Impl> m_impl;

        protected:
            InputMeter();
            ~InputMeter();

            void update(float dt) override;

            bool init() override;

        public:
            static InputMeter* create();

            void playPause(bool play);

            bool isDeath() const noexcept;
            bool isDanger() const noexcept;
        };
    };
};