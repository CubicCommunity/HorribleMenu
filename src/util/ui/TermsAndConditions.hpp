#pragma once

#include <Geode/Geode.hpp>

namespace horrible {
    namespace ui {
        class TermsAndConditions final : public geode::Popup {
            using Callback = geode::CopyableFunction<void(bool)>;

        protected:
            bool init(Callback&& cb);

        public:
            static TermsAndConditions* create(Callback&& cb);
        };
    };
};