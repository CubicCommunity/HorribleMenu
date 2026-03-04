#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace horrible {
    namespace ui {
        class MathQuiz final : public CCBlockLayer, public FLAlertLayerProtocol {
        private:
            class Impl;
            std::unique_ptr<Impl> m_impl;

            enum class MathOperation : unsigned int {
                Addition = 0,
                Subtraction = 1,
                Multiplication = 2,
                Geometry = 3
            };

        protected:
            MathQuiz();
            ~MathQuiz();

            void closeAfterFeedback(CCNode* node);
            void onAnswerClicked(CCObject* sender);

            bool hasAnswer(int answer) const noexcept;

            void keyBackClicked() override;
            void update(float dt) override;

            bool init() override;

        public:
            static MathQuiz* create();

            void setCallback(Function<void(bool)> cb);
            void setCorrect(bool v);
        };

        class Richard final : public CCNode {
        protected:
            bool init() override;

        public:
            static Richard* create();
        };
    };
};