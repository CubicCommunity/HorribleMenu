#pragma once

#include <Geode/Geode.hpp>

#include <Geode/ui/Button.hpp>

using namespace geode::prelude;

namespace horrible {
    namespace ui {
        class Captcha final : public Popup {
            using Callback = Function<void(bool)>;

        private:
            struct Impl;
            std::unique_ptr<Impl> m_impl;

        protected:
            Captcha();
            ~Captcha();

            void callAfterFeedback(float);
            void setSuccess(bool v);

            void update(float dt) override;

            bool init() override;

        public:
            static Captcha* create();

            void setCallback(Callback&& cb);
        };

        class RobotVerifier final : public CCNode {
            using Callback = Function<void(bool)>;

        private:
            std::string m_correct = "";
            Callback m_callback = nullptr;

            void addNewBtn();
            void validateBtns(geode::Button* called);

        protected:
            bool init(std::string id, Callback&& cb);

        public:
            static RobotVerifier* create(std::string id, Callback&& cb);
        };
    };
};