#pragma once

#include <Geode/platform/platform.hpp>

// Container for Horrible Ideas API
namespace horrible {
    // Alias for `geode::PlatformID`
    using Platform = geode::PlatformID;

    // How silly an option is
    enum class SillyTier : unsigned int {
        None = 0,    // Null
        Low = 1,     // Not so silly
        Medium = 2,  // Somewhat silly
        High = 3     // Very silly
    };

    // A horrible option
    struct Option final {
        std::string id;                   // Unique ID of the option
        std::string name;                 // Name of the option
        std::string description;          // Description of the option
        std::string category;             // Name of the category this option should be under
        SillyTier silly;                  // How silly the option is
        bool restart;                     // If the option requires a restart to take effect
        std::vector<Platform> platforms;  // Platforms that the option supports

        Option() = default;  // Default constructor

        // Constructor
        Option(
            std::string id,
            std::string name,
            std::string description,
            std::string category,
            SillyTier silly = SillyTier::Low,
            bool restart = false,
            std::vector<Platform> platforms = {Platform::Desktop, Platform::Mobile}) :
            id(std::move(id)),
            name(std::move(name)),
            description(std::move(description)),
            category(std::move(category)),
            silly(silly),
            restart(restart),
            platforms(std::move(platforms)) {};
    };
};