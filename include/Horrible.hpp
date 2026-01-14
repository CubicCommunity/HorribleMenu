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
    class AWCW_HORRIBLE_API_DLL OptionManager : public cocos2d::CCObject {
    private:
        class Impl; // PImpl class
        std::unique_ptr<Impl> m_impl; // PImpl pointer

    protected:
        OptionManager(); // Constructor
        virtual ~OptionManager(); // Destructor

        /**
         * Register a category if not already registered
         *
         * @param category Name of the category
         */
        void registerCategory(std::string_view category);

        /**
         * Check if an option already exists
         *
         * @param id The ID of the option to check
         *
         * @returns Whether this option already exists or not
         */
        bool doesOptionExist(std::string_view id) const;

        friend class OptionEventFilter;

    public:
        // Get option manager singleton
        static OptionManager* get();

        /**
         * Register a new option
         *
         * @param option Constructed option object
         */
        void registerOption(Option const& option);

        /**
         * Returns a reference to the array of all registered options
         *
         * @returns An array of every registered option, main and external
         */
        std::vector<Option> const& getOptions() const;

        /**
         * Returns the toggle state of an option
         *
         * @param id The ID of the option to check
         *
         * @returns Boolean of the current value
         */
        bool getOption(std::string_view id) const;

        /**
         * Set the toggle state of an option
         *
         * @param id The ID of the option to toggle
         * @param enable Boolean to toggle to
         *
         * @returns Boolean of the old value
         */
        bool setOption(std::string_view id, bool enable) const;

        /**
         * Returns a reference to the array of all registered categories
         *
         * @returns An array of every category name
         */
        std::vector<std::string> const& getCategories() const;
    };
};