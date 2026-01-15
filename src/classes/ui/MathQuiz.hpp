#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace horrible {
    namespace ui {
        class MathQuiz : public CCBlockLayer, public FLAlertLayerProtocol {
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
            virtual ~MathQuiz();

            void closeAfterFeedback(CCNode* node);
            void onAnswerClicked(CCObject* sender);

            bool hasAnswer(int answer) const noexcept;

            void keyBackClicked() override;
            void update(float dt) override;

            bool init() override;

        public:
            static MathQuiz* create();

            void setCallback(std::function<void(bool)> const& cb);
            void setCorrect(bool v);

            void closePopup();
        };

        class Richard : public CCNode {
        private:
            class Impl;
            std::unique_ptr<Impl> m_impl;

        protected:
            Richard();
            virtual ~Richard();

            bool init() override;

        public:
            static Richard* create();
        };
    };
};