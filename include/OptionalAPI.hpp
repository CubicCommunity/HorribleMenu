#pragma once

#include "Horrible.hpp"

#include <Geode/Result.hpp>

#include <Geode/loader/Event.hpp>
#include <Geode/loader/Dispatch.hpp>

#ifdef MY_MOD_ID
#undef MY_MOD_ID
#endif
#define MY_MOD_ID "arcticwoof.horribleideas"

namespace horrible {
    struct OptionEventV2 final : geode::Event<OptionEventV2, bool(HorribleOptionSave), std::string> {
        using Event::Event;
    };

    class OptionManagerV2 final {
    public:
        static geode::Result<> registerOption(Option option)
            GEODE_EVENT_EXPORT(&OptionManagerV2::registerOption, (option));

        [[nodiscard]] static geode::Result<bool> isEnabled(std::string_view id)
            GEODE_EVENT_EXPORT(&OptionManagerV2::isEnabled, (id));

        static geode::Result<> setOption(geode::ZStringView id, bool enable, bool pin = false)
            GEODE_EVENT_EXPORT(&OptionManagerV2::setOption, (id, enable, pin));
    };
};