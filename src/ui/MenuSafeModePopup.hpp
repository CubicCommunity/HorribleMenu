#pragma once

#include <Util.h>

#include <Geode/Geode.hpp>

namespace cs::brkd::horrible {
    class MenuSafeModePopup final : public geode::Popup {
    protected:
        bool init() override;

    public:
        static MenuSafeModePopup* create();
    };
};