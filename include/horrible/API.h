#pragma once

#include "DLL.hpp"

#include "Events.hpp"
#include "Option.hpp"

#include <cocos2d.h>

#include <matjson.hpp>

#include <Geode/Result.hpp>

#include <Geode/utils/function.hpp>
#include <Geode/utils/StringMap.hpp>
#include <Geode/utils/ZStringView.hpp>

template <>
struct matjson::Serialize<HorribleOptionSave> final {
    static geode::Result<HorribleOptionSave> fromJson(matjson::Value const& value);
    static matjson::Value toJson(HorribleOptionSave const& value);
};

// Container for Horrible Menu API functions
namespace horrible {
    template <class T>
    using HashedMapU64 = std::unordered_map<uint64_t, T>;

    // Option manager for Horrible Menu
    class BRKD_HORRIBLE_API_DLL OptionManager final {
        friend class Option;

        // Type alias for `geode::Function<void(bool)>`, used in hook delegation
        using Callback = geode::Function<void(bool)>;

    private:
        geode::utils::StringMap<SharedOption> m_options;  // Map of registered options
        std::vector<std::string> m_categories;            // Array of auto-registered categories

        geode::utils::StringMap<std::vector<SharedOption>> m_categoryMap;  // Map of options bound to category name
        geode::utils::StringMap<const geode::Mod* const> m_integrations;   // Map of auto-registered external mods using this API

        HashedMapU64<asp::BoxedString> m_optHashes;  // Map of FNV-1a-hashed option IDs to their string IDs
        HashedMapU64<OptionSave> m_saveCache;        // Map of cached states

        HashedMapU64<std::vector<Callback>> m_delegates;  // Map of option ID to array of delegates to call when that option is toggled
        std::unordered_set<uint64_t> m_enabledCheats;     // Map of currently enabled cheat options, used for dynamic safe mode

    protected:
        OptionManager() = default;
        ~OptionManager() = default;

        OptionManager(const OptionManager&) = delete;
        OptionManager& operator=(const OptionManager&) = delete;

        OptionManager(OptionManager&&) = delete;
        OptionManager& operator=(OptionManager&&) = delete;

        /// Register a category if not already registered
        /// @param category Name of the category
        void registerCategory(std::string category, SharedOption option);

        /// Register an external Geode mod as an integration if not already registered
        /// @param category Pointer to the mod
        void registerMod(const geode::Mod*);

        /// Check if an option already exists
        /// @param id The ID of the option to check
        bool doesOptionExist(geode::ZStringView id) const noexcept;

        /// Check if an external Geode mod has already been registered in the list of integrations
        /// @param id The ID of the option to check
        bool isModRegistered(geode::ZStringView id) const noexcept;

    public:
        // Get option manager singleton
        static OptionManager* get() noexcept;

        /// Register a new option
        /// @param option Constructed option object
        void registerOption(SharedOption option);

        /// Check if a cheat option is currently enabled
        bool isCheatEnabled() const noexcept;

        /// Returns a array with references to all registered options
        /// @deprecated Will be replaced with `OptionManager::getAllOptions` in GD 2.209x ports
        [[nodiscard]] [[deprecated("Use `OptionManager::getAllOptions` instead")]] std::vector<std::weak_ptr<Option>> getOptions() const;
        geode::utils::StringMap<SharedOption> const& getAllOptions() const noexcept;

        /// Returns a reference to a vector of options for a specific category
        /// @param category The name of the category to check for its assigned options
        std::span<const SharedOption> getAllOptionsForCategory(geode::ZStringView category) const noexcept;

        /// Quickly check the toggle state of an option
        /// @param id The ID of the option to check
        [[nodiscard]] bool isEnabled(geode::ZStringView id) const;
        [[nodiscard]] bool isEnabled(uint64_t id) const;

        /// Quickly check the pin state of an option
        /// @param id The ID of the option to check
        [[nodiscard]] bool isPinned(geode::ZStringView id) const;
        [[nodiscard]] bool isPinned(uint64_t id) const;

        /// Quickly check the viewed state of an option
        /// @param id The ID of the option to check
        [[nodiscard]] bool isViewed(geode::ZStringView id) const;
        [[nodiscard]] bool isViewed(uint64_t id) const;

        /// Quickly check if an option is a cheat option
        /// @param id The ID of the option to check
        [[nodiscard]] bool isCheating(geode::ZStringView id) const;

        /// Quickly check the default toggle state of an option
        /// @param id The ID of the option to check
        [[nodiscard]] bool getDefaultToggleState(geode::ZStringView id) const noexcept;

        /// Get the saved data of an option
        /// @param id The ID of the option to check
        [[nodiscard]] OptionSave getOption(geode::ZStringView id) const;
        [[nodiscard]] OptionSave getOption(uint64_t id) const;

        /// Returns the data of an option
        /// @param id The ID of the option to get
        [[nodiscard]] std::weak_ptr<Option> getOptionInfo(geode::ZStringView id) const noexcept;
        [[nodiscard]] std::weak_ptr<Option> getOptionInfo(uint64_t id) const noexcept;

        /// Returns the string ID of an option via hash ID lookup
        /// @param id The hash ID of the option to get
        [[nodiscard]] geode::Result<asp::BoxedString> getOptionIDForHash(uint64_t id) const noexcept;

        /// Returns the amount of delegate callbacks registered for an option
        /// @param id The ID of the option whose callbacks to check
        [[nodiscard]] size_t getDelegateCount(std::string_view id) const noexcept;
        [[nodiscard]] size_t getDelegateCount(uint64_t id) const noexcept;

        /// Check if Safe Mode should be enabled based on the current state of options and settings
        [[nodiscard]] bool shouldBeSafeMode() const noexcept;

        /// Set the toggle state of an option
        /// @param id The ID of the option to toggle
        /// @param enable Boolean to toggle to
        void toggleOption(geode::ZStringView id, bool enable);
        void toggleOption(uint64_t id, bool enable);

        /// Set the state of an option
        /// @param id The ID of the option to toggle
        /// @param enable Boolean to toggle to
        /// @param pin If this option is pinned by the user
        /// @param viewed If this option was already viewed by the user
        void setOption(geode::ZStringView id, bool enable, bool pin = false, bool viewed = true);
        void setOption(uint64_t id, bool enable, bool pin = false, bool viewed = true);

        /// Upsert a new hook delegate
        /// @param id The ID of the option to set the delegate for
        /// @param callback The hook callback to register for this option's delegate
        void addDelegate(geode::ZStringView id, Callback&& callback);
        void addDelegate(uint64_t id, Callback&& callback);

        /// Returns a reference to the vector of all registered categories
        /// @deprecated Will be replaced with `OptionManager::getAllCategories` in GD 2.209x ports
        [[nodiscard]] [[deprecated("Use `OptionManager::getAllCategories` instead")]] std::span<const std::string> getCategories() const noexcept;
        geode::utils::StringMap<std::vector<SharedOption>> const& getAllCategories() const noexcept;

        /// Returns a vector of all registered Geode mod integrations
        [[nodiscard]] std::vector<const geode::Mod*> getMods() const;
    };

    /// Delegate hooks to OptionManager for dynamic toggling
    /// @param id The ID of the option to delegate for
    /// @param hooks The map of hooks to delegate
    BRKD_HORRIBLE_API_DLL void delegateHooks(geode::ZStringView id, geode::utils::StringMap<std::shared_ptr<geode::Hook>> const& hooks);

    /// Check whether the current user supports Breakeode on Ko-fi
    BRKD_HORRIBLE_API_DLL bool isSupporter() noexcept;
};

// Statically register an option
#define HORRIBLE_REGISTER_OPTION(opt)                                          \
    $on_mod(Loaded) {                                                          \
        if (auto om = horrible::OptionManager::get()) om->registerOption(opt); \
    }

#define HORRIBLE_DELEGATE_HOOKS_FUNC(optID) horrible::delegateHooks(optID, self.m_hooks)

// Delegate hooks to OptionManager for dynamic toggling
#define HORRIBLE_DELEGATE_HOOKS(optID)       \
    static void onModify(auto& self) {       \
        HORRIBLE_DELEGATE_HOOKS_FUNC(optID); \
    }