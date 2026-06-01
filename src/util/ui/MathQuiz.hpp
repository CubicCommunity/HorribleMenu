#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace horrible {
    namespace ui {
        class MathQuiz final : public CCBlockLayer {
            using Callback = Function<void(bool)>;

            enum class MathOperation : uint8_t {
                Addition = 0,
                Subtraction = 1,
                Multiplication = 2,
                Geometry = 3
            };

        private:
            struct Impl;
            std::unique_ptr<Impl> m_impl;

        protected:
            MathQuiz();
            ~MathQuiz();

            void callAfterFeedback(CCNode* node);

            void setCorrect(bool v);
            bool hasAnswer(int answer) const noexcept;

            void update(float dt) override;

            bool init() override;

        public:
            static MathQuiz* create();

            void setCallback(Callback&& cb);
        };

        class Richard final : public CCNode {
        protected:
            bool init() override;

        public:
            static Richard* create();
        };
    };
};