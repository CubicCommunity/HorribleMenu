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
    // Event for option toggles
    struct OptionEvent final : geode::Event<OptionEvent, bool(HorribleOptionSave), std::string> {
        using Event::Event;
    };

    // Optional bridge to option manager for Horrible Ideas
    class OptionManagerV2 final {
    public:
        /**
         * Register a new option
         *
         * @param option Constructed option object
         */
        static void registerOption(Option option)
            GEODE_EVENT_EXPORT_NORES(&OptionManagerV2::registerOption, (std::move(option)));

        /**
         * Quickly check the toggle state of an option
         *
         * @param id The ID of the option to check
         *
         * @returns Boolean of the current value
         */
        [[nodiscard]] static geode::Result<bool> isEnabled(std::string_view id)
            GEODE_EVENT_EXPORT(&OptionManagerV2::isEnabled, (id));

        /**
         * Set the toggle state of an option
         *
         * @param id The ID of the option to toggle
         * @param enable Boolean to toggle to
         * @param pin If this option is a user pin
         */
        static void setOption(geode::ZStringView id, bool enable, bool pin = false)
            GEODE_EVENT_EXPORT_NORES(&OptionManagerV2::setOption, (id, enable, pin));
    };
};