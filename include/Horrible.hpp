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

#include "Events.hpp"
#include "Option.hpp"

#include <cocos2d.h>

// Container for Horrible Ideas API functions
namespace horrible {
    // Mod option manager for Horrible Ideas
    class AWCW_HORRIBLE_API_DLL OptionManager final : public cocos2d::CCObject {
    private:
        std::vector<Option> m_options; // Array of registered options
        std::vector<std::string> m_categories; // Array of auto-registered categories

    protected:
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
         * Set the toggle state of an option
         *
         * @param id The ID of the option to toggle
         * @param enable Boolean to toggle to
         *
         * @returns Boolean of the old value
         */
        bool setOption(geode::ZStringView id, bool enable) const;

        /**
         * Returns a reference to the array of all registered categories
         *
         * @returns An array of every category name
         */
        [[nodiscard]] std::span<const std::string> getCategories() const noexcept;
    };
};

// Statically register an option
#define REGISTER_HORRIBLE_OPTION(opt) $execute {\
    if (auto om = horrible::OptionManager::get()) om->registerOption(opt); \
}