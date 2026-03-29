#pragma once

#include "DLL.hpp"

#include <Geode/utils/ZStringView.hpp>

#include <Geode/platform/platform.hpp>

// Container for Horrible Menu API
namespace horrible {
    // Alias for `geode::PlatformID`
    using Platform = geode::PlatformID;

    // How silly an option is
    enum class SillyTier : unsigned int {
        None = 0,    // Null, act as placeholder
        Low = 1,     // Not so silly
        Medium = 2,  // Somewhat silly
        High = 3     // Very silly
    };

    // Metadata for a horrible option
    struct AWCW_HORRIBLE_API_DLL Option final {
    private:
        std::string m_id = "";                                // Unique ID of the option
        std::string m_name = "Example Option";                // Name of the option
        std::string m_description = "";                       // Description of the option
        std::string m_category = "Uncategorized";             // Name of the category this option should be under
        SillyTier m_silly = SillyTier::None;                  // How silly the option is
        bool m_restart = false;                               // If the option requires a restart to take effect
        std::vector<Platform> m_platforms = {Platform::All};  // Platforms that the option supports

    public:
        Option() = default;  // Default constructor

        static Option create(std::string id);

        Option& setID(std::string id);
        Option& setName(std::string name);
        Option& setDescription(std::string description);
        Option& setCategory(std::string category);
        Option& setSillyTier(SillyTier tier);
        Option& setRequiresRestart(bool required);
        Option& setSupportedPlatforms(std::vector<Platform> platforms);

        geode::ZStringView getID() const noexcept;
        geode::ZStringView getName() const noexcept;
        geode::ZStringView getDescription() const noexcept;
        geode::ZStringView getCategory() const noexcept;
        SillyTier getSillyTier() const noexcept;
        bool isRestartRequired() const noexcept;
        std::span<const Platform> getSupportedPlatforms() const noexcept;
    };
};