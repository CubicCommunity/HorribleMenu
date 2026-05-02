#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace horrible {
    namespace ui {
        class WhackButton final : public CCNode {
            using Callback = Function<void(bool)>;

        private:
            class Impl;
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

            void setCallback(Callback&& cb) &;
        };
    };
};