#pragma once
#include <Geode/Geode.hpp>
#include "Geode/ui/Popup.hpp"
#include "Geode/ui/Button.hpp"

using namespace geode::prelude;

namespace horrible {
    namespace ui {
        class TermsAndCondition final : public geode::Popup {
        protected:
            ~TermsAndCondition();
            bool init();

            MDTextArea* m_tosArea = nullptr;
            geode::Button* m_acceptButton = nullptr;
            geode::Button* m_declineButton = nullptr;

            void onDecline();
            void onAccept();

            bool m_accepted = false;

        public:
            static TermsAndCondition* create();
        };
    }
};