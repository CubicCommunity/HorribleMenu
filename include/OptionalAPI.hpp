#pragma once

#include "Horrible.h"

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

    // Metadata for a horrible option
    struct OptionV2 final {
        std::string id;                   // Unique ID of the option
        std::string name;                 // Name of the option
        std::string description;          // Description of the option
        std::string category;             // Name of the category this option should be under
        SillyTier silly;                  // How silly the option is
        bool restart;                     // If the option requires a restart to take effect
        std::vector<Platform> platforms;  // Platforms that the option supports

        OptionV2() = default;  // Default constructor

        // Constructor
        inline OptionV2(
            std::string id,
            std::string name,
            std::string description,
            std::string category,
            SillyTier silly = SillyTier::Low,
            bool restart = false,
            std::vector<Platform> platforms = {Platform::All}) :
            id(std::move(id)),
            name(std::move(name)),
            description(std::move(description)),
            category(std::move(category)),
            silly(silly),
            restart(restart),
            platforms(std::move(platforms)) {};
    };

    // Optional bridge to option manager for Horrible Ideas
    class OptionManagerV2 final {
    public:
        /**
         * Register a new option
         *
         * @param option Constructed option object
         */
        static void registerOption(OptionV2 const& option)
            GEODE_EVENT_EXPORT_NORES(&OptionManagerV2::registerOption, (option));

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
         */
        static void toggleOption(geode::ZStringView id, bool enable)
            GEODE_EVENT_EXPORT_NORES(&OptionManagerV2::toggleOption, (id, enable));
    };
};