#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace horrible {
    namespace ui {
        class WhackButton final : public CCNode {
        private:
            class Impl;
            std::unique_ptr<Impl> m_impl;

            using Callback = Function<void(bool)>;

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