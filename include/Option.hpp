#pragma once

#include "Horrible.hpp"

#include <Geode/Geode.hpp>

// Container for Horrible Ideas API
namespace horrible {
    // How silly an option is
    enum class SillyTier : unsigned int {
        None = 0, // Null
        Low = 1, // Not so silly
        Medium = 2, // Somewhat silly
        High = 3 // Very silly
    };

    // A horrible option
    struct Option {
        std::string id; // Unique ID of the option
        std::string name; // Name of the option
        std::string description; // Description of the option
        std::string category; // Name of the category this option should be under
        SillyTier silly; // How silly the option is
        bool restart; // If the option requires a restart to take effect
        std::vector<geode::PlatformID> platforms; // Platforms that the option supports

        Option() = default; // Default constructor

        // Constructor
        Option(
            std::string id,
            std::string name,
            std::string description,
            std::string category,
            SillyTier silly = SillyTier::Low,
            bool restart = false,
            std::vector<geode::PlatformID> platforms = { geode::PlatformID::Desktop, geode::PlatformID::Mobile }
        ) : id(std::move(id)),
            name(std::move(name)),
            description(std::move(description)),
            category(std::move(category)),
            silly(silly),
            restart(restart),
            platforms(std::move(platforms)) {};
    };
};