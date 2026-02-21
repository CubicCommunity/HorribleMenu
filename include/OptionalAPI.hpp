#pragma once

#include "Horrible.hpp"

#include <Geode/Result.hpp>

#include <Geode/loader/Event.hpp>
#include <Geode/loader/Dispatch.hpp>

#ifdef MY_MOD_ID
#undef MY_MOD_ID
#endif
#define MY_MOD_ID "arcticwoof.horrible_ideas"

namespace horrible {
    struct OptionEventV2 final : geode::Event<OptionEventV2, bool(bool), std::string> {
        using Event::Event;
    };

    class OptionManagerV2 final {
    public:
        static geode::Result<> registerOption(Option const& option)
            GEODE_EVENT_EXPORT(&OptionManagerV2::registerOption, (option));

        [[nodiscard]] static geode::Result<bool> getOption(std::string_view id)
            GEODE_EVENT_EXPORT(&OptionManagerV2::getOption, (id));

        static geode::Result<bool> setOption(geode::ZStringView id, bool enable)
            GEODE_EVENT_EXPORT(&OptionManagerV2::setOption, (id, enable));
    };
};