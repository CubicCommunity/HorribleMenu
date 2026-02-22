#pragma once

#ifdef GEODE_IS_WINDOWS
#ifdef AWCW_HORRIBLE_API_EXPORTING
#define AWCW_HORRIBLE_API_DLL __declspec(dllexport)
#else
#define AWCW_HORRIBLE_API_DLL __declspec(dllimport)
#endif
#else
#ifdef AWCW_HORRIBLE_API_EXPORTING
#define AWCW_HORRIBLE_API_DLL __attribute__((visibility("default")))
#else
#define AWCW_HORRIBLE_API_DLL
#endif
#endif

#include "Option.hpp"

#include <cocos2d.h>

#include <Geode/Result.hpp>

#include <Geode/utils/function.hpp>
#include <Geode/utils/StringMap.hpp>
#include <Geode/utils/ZStringView.hpp>

// Container for Horrible Ideas API functions
namespace horrible {
    // Mod option manager for Horrible Ideas
    class AWCW_HORRIBLE_API_DLL OptionManager final : public cocos2d::CCObject {
    private:
        std::vector<Option> m_options; // Array of registered options
        std::vector<std::string> m_categories; // Array of auto-registered categories
        std::unordered_map<std::string, std::vector<geode::Function<void(bool)>>> m_delegates; // Map of option ID to array of delegates to call when that option is toggled

    protected:
        OptionManager() = default; // Default constructor

        OptionManager(const OptionManager&) = delete; // No copying
        OptionManager& operator=(const OptionManager&) = delete; // No copy assignment

        /**
         * Register a category if not already registered
         *
         * @param category Name of the category
         */
        void registerCategory(std::string category);

        /**
         * Check if an option already exists
         *
         * @param id The ID of the option to check
         *
         * @returns Whether this option already exists or not
         */
        bool doesOptionExist(std::string_view id) const noexcept;

    public:
        // Get option manager singleton
        static OptionManager* get() noexcept;

        /**
         * Register a new option
         *
         * @param option Constructed option object
         */
        void registerOption(Option option);

        /**
         * Returns a reference to the array of all registered options
         *
         * @returns An array of every registered option, main and external
         */
        [[nodiscard]] std::span<const Option> getOptions() const noexcept;

        /**
         * Returns the toggle state of an option
         *
         * @param id The ID of the option to check
         *
         * @returns Boolean of the current value
         */
        [[nodiscard]] bool getOption(std::string_view id) const noexcept;

        /**
         * Returns the data of an option
         *
         * @param id The ID of the option to get
         *
         * @returns A result possibly containing the option object
         */
        [[nodiscard]] geode::Result<Option> getOptionInfo(std::string_view id) const noexcept;

        /**
         * Returns the amount of delegate callbacks registered for an option
         *
         * @param id The ID of the option whose callbacks to check
         *
         * @returns The amount of callbacks registered for this option
         */
        [[nodiscard]] size_t getDelegateCount(std::string_view id) const noexcept;

        /**
         * Set the toggle state of an option
         *
         * @param id The ID of the option to toggle
         * @param enable Boolean to toggle to
         *
         * @returns Boolean of the old value
         */
        bool setOption(geode::ZStringView id, bool enable);

        /**
         * Upsert a new hook delegate
         *
         * @param id The ID of the option to set the delegate for
         * @param callback The hook callback to register for this option's delegate
         */
        void addDelegate(geode::ZStringView id, geode::Function<void(bool)>&& callback);

        /**
         * Returns a reference to the array of all registered categories
         *
         * @returns An array of every category name
         */
        [[nodiscard]] std::span<const std::string> getCategories() const noexcept;
    };

    /**
     * Delegate hooks to OptionManager for dynamic toggling
     *
     * @param id The ID of the option to delegate for
     * @param hooks The map of hooks to delegate
     */
    AWCW_HORRIBLE_API_DLL void delegateHooks(geode::ZStringView id, geode::utils::StringMap<std::shared_ptr<geode::Hook>>& hooks);
};

// Statically register an option
#define HORRIBLE_REGISTER_OPTION(opt) \
$execute { \
    if (auto om = horrible::OptionManager::get()) om->registerOption(opt); \
}

// Delegate hooks to OptionManager for dynamic toggling
#define HORRIBLE_DELEGATE_HOOKS(id) \
static void onModify(auto& self) { \
    horrible::delegateHooks(id, self.m_hooks); \
}