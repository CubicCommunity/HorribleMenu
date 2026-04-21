#pragma once

#include "DLL.hpp"

#include <Geode/utils/ZStringView.hpp>

#include <Geode/loader/Mod.hpp>

#include <Geode/platform/platform.hpp>

struct HorribleOptionSave final {
    bool enabled = false;
    bool pin = false;
    bool viewed = false;
};

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
    struct BRKD_HORRIBLE_API_DLL Option final : std::enable_shared_from_this<Option> {
    private:
        std::string m_id = "id"_spr;                          // Unique ID of the option
        std::string m_name = "Example Option";                // Name of the option
        std::string m_description = "";                       // Description of the option
        std::string m_category = "Uncategorized";             // Name of the category this option should be under
        SillyTier m_silly = SillyTier::None;                  // How silly the option is
        bool m_default = false;                               // Default toggle state for this option
        bool m_online = false;                                // If the option requires an active internet connection to work properly
        bool m_restart = false;                               // If the option requires a game restart to take effect
        std::vector<Platform> m_platforms = {Platform::All};  // Platforms that the option supports
        const geode::Mod* const m_integration = nullptr;      // External mod that registered this option

    public:
        Option(std::string id, const geode::Mod* integration = geode::Mod::get());

        static std::shared_ptr<Option> create(std::string id, const geode::Mod* integration = geode::Mod::get());

        std::shared_ptr<Option> setID(std::string id) &;
        std::shared_ptr<Option> setName(std::string name) &;
        std::shared_ptr<Option> setDescription(std::string description) &;
        std::shared_ptr<Option> setCategory(std::string category) &;
        std::shared_ptr<Option> setSillyTier(SillyTier tier) &;
        std::shared_ptr<Option> setDefaultToggleState(bool state) &;
        std::shared_ptr<Option> setOnline(bool online) &;
        std::shared_ptr<Option> setRequiresRestart(bool required) &;
        std::shared_ptr<Option> setSupportedPlatforms(std::vector<Platform> platforms) &;

        std::shared_ptr<Option> autoRegister() &;

        [[nodiscard]] geode::ZStringView getID() const noexcept;
        [[nodiscard]] geode::ZStringView getName() const noexcept;
        [[nodiscard]] geode::ZStringView getDescription() const noexcept;
        [[nodiscard]] geode::ZStringView getCategory() const noexcept;
        [[nodiscard]] bool getDefaultToggleState() const noexcept;
        [[nodiscard]] SillyTier getSillyTier() const noexcept;
        [[nodiscard]] bool isOnline() const noexcept;
        [[nodiscard]] bool isRestartRequired() const noexcept;
        [[nodiscard]] std::span<const Platform> getSupportedPlatforms() const noexcept;
        [[nodiscard]] const geode::Mod* getIntegration() const noexcept;

        void enable();
        void disable();

        [[nodiscard]] bool isEnabled() const;
        [[nodiscard]] bool isPinned() const;
    };
};