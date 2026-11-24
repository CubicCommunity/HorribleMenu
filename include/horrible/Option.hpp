#pragma once

#include "DLL.hpp"

#include <asp/ptr/BoxedString.hpp>

#include <Geode/utils/ZStringView.hpp>

#include <Geode/loader/Mod.hpp>

#include <Geode/platform/platform.hpp>

struct HorribleOptionSave final {
    bool enabled = false;
    bool pin = false;
    bool viewed = false;
};

// Container for Horrible Menu API
namespace cs::brkd::horrible {
    // Alias for `HorribleOptionSave`
    using OptionSave = HorribleOptionSave;

    // Alias for `geode::PlatformID`
    using Platform = geode::PlatformID;

    // How silly an option is
    enum class SillyTier : uint8_t {
        None = 0,    // Null, act as placeholder
        Low = 1,     // Not so silly
        Medium = 2,  // Somewhat silly
        High = 3     // Very silly
    };

    // Metadata for a horrible option
    struct BRKD_HORRIBLE_API_DLL Option final : std::enable_shared_from_this<Option> {
        // Alias for `std::shared_ptr<Option>`
        using SharedSelf = std::shared_ptr<Option>;

    private:
        asp::BoxedString const m_id;                            // Unique ID of the option
        std::string m_name = "Example Option";                  // Name of the option
        std::string m_description;                              // Description of the option
        std::string m_category = "Uncategorized";               // Name of the category this option should be under
        SillyTier m_silly = SillyTier::None;                    // How silly the option is
        bool m_default = false;                                 // Default toggle state for this option
        bool m_online = false;                                  // If the option requires an active internet connection to work properly
        bool m_restart = false;                                 // If the option requires a game restart to take effect
        std::vector<Platform> m_platforms = { Platform::All };  // Platforms that the option supports
        bool m_isCheating = false;                              // If the option counts as cheating and will trigger dynamic safe mode
        const geode::Mod* const m_integration = nullptr;        // External mod that registered this option

        uint64_t m_idHash = 0;  // FNV-1a-hashed unique ID of the option

    public:
        Option(std::string id, const geode::Mod* integration = geode::Mod::get());

        /// Create a new option metadata object
        /// @param id Unique ID for this option
        /// @param integration Pointer to the mod this option is being registered from
        static SharedSelf create(std::string id, const geode::Mod* integration = geode::Mod::get());

        SharedSelf setName(std::string name);
        SharedSelf setDescription(std::string description);
        SharedSelf setCategory(std::string category);
        SharedSelf setSillyTier(SillyTier tier);
        SharedSelf setDefaultToggleState(bool state);
        SharedSelf setOnline(bool online);
        SharedSelf setRequiresRestart(bool required);
        SharedSelf setSupportedPlatforms(std::vector<Platform> platforms);
        SharedSelf setCheating(bool cheat);

        SharedSelf autoRegister();

        geode::ZStringView getID() const noexcept;
        geode::ZStringView getName() const noexcept;
        geode::ZStringView getDescription() const noexcept;
        geode::ZStringView getCategory() const noexcept;
        bool getDefaultToggleState() const noexcept;
        SillyTier getSillyTier() const noexcept;
        bool isOnline() const noexcept;
        bool isRestartRequired() const noexcept;
        std::span<const Platform> getSupportedPlatforms() const noexcept;
        bool isCheating() const noexcept;
        const geode::Mod* getIntegration() const noexcept;

        asp::BoxedString getIDShared() const noexcept;
        uint64_t getIDHash() const noexcept;

        void enable() &;
        void disable() &;

        bool isEnabled() const&;
        bool isPinned() const&;
    };

    // Alias for `std::shared_ptr<Option>`
    using SharedOption = Option::SharedSelf;
};