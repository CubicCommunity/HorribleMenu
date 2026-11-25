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
        None,    // Null, act as placeholder
        Low,     // Not so silly
        Medium,  // Somewhat silly
        High     // Very silly
    };

    enum class IncompatType : uint8_t {
        None,       // Perfectly fine
        Platform,   // Cannot work for the current platform
        Internet,   // Requires internet connection
        Anticheat,  // Disabled by anti-cheat
    };

    // Metadata for a horrible option
    struct BRKD_HORRIBLE_API_DLL Option final : std::enable_shared_from_this<Option> {
        // Alias for `std::shared_ptr<Option>`
        using SharedSelf = std::shared_ptr<Option>;

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;

    public:
        Option(std::string id, const geode::Mod* integration = geode::Mod::get());
        ~Option();

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
        SharedSelf setSupportedPlatforms(std::unordered_set<Platform> platforms);
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
        std::unordered_set<Platform> const& getSupportedPlatforms() const noexcept;
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