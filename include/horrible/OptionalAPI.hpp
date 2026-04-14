#pragma once

#include "API.h"

#include <Geode/Result.hpp>

#include <Geode/loader/Mod.hpp>
#include <Geode/loader/Dispatch.hpp>

#ifdef MY_MOD_ID
#undef MY_MOD_ID
#endif
#define MY_MOD_ID "cubicstudios.horriblemenu"

namespace horrible {
    // Metadata for a horrible option
    struct OptionV2 final {
    private:
        const geode::Mod* const integration = nullptr;  // External mod that registered this option

    public:
        std::string id;                   // Unique ID of the option
        std::string name;                 // Name of the option
        std::string description;          // Description of the option
        std::string category;             // Name of the category this option should be under
        SillyTier silly;                  // How silly the option is
        bool state;                       // Default toggle state for this option
        bool online;                      // If the option requires an active internet connection to work properly
        bool restart;                     // If the option requires a game restart to take effect
        std::vector<Platform> platforms;  // Platforms that the option supports

        OptionV2() = default;  // Default constructor

        // Constructor
        inline OptionV2(
            std::string id,
            std::string name,
            std::string description,
            std::string category,
            SillyTier silly = SillyTier::Low,
            bool state = false,
            bool online = false,
            bool restart = false,
            std::vector<Platform> platforms = {Platform::All}) :
            id(std::move(id)),
            name(std::move(name)),
            description(std::move(description)),
            category(std::move(category)),
            silly(silly),
            state(state),
            online(online),
            restart(restart),
            platforms(std::move(platforms)),
            integration(geode::Mod::get()) {};

        inline const geode::Mod* getIntegration() const noexcept {
            return integration;
        };
    };

    // Optional bridge to option manager for Horrible Menu
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
        [[nodiscard]] static geode::Result<bool> isEnabled(geode::ZStringView id)
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