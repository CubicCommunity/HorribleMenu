#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace horrible {
    namespace ui {
        class TermsAndConditions final : public Popup {
        private:
            using Callback = CopyableFunction<void(bool)>;

        protected:
            bool init(Callback&& cb);

        public:
            static TermsAndConditions* create(Callback&& cb);
        };
    };
};