#pragma once

#include <Utils.hpp>

namespace horrible {
    namespace util {
        // Horrible options utilities
        namespace options {
            /**
             * Returns the array of all registered options
             *
             * @returns An array of every registered option, main and external
             */
            std::span<const Option> getAll() noexcept;

            /**
             * Returns the toggle state of an option
             *
             * @param id The ID of the option to check
             *
             * @returns Boolean of the current value
             */
            bool get(std::string_view id) noexcept;

            /**
             * Returns the chance value for an option
             *
             * @param id The ID of the option to check
             *
             * @returns Integer of the chance value
             */
            int getChance(std::string_view id);

            /**
             * Set the toggle state of an option
             *
             * @param id The ID of the option to toggle
             * @param enable Boolean to toggle to
             *
             * @returns Boolean of the old value
             */
            bool set(std::string_view id, bool enable);

            /**
             * Returns the array of all registered option categories
             *
             * @returns An array of every registered option category, main and external
             */
            std::span<const std::string> getAllCategories() noexcept;

            /**
             * Returns if a category exists or not
             *
             * @param category The exact name of the category to check
             */
            bool doesCategoryExist(std::string_view category) noexcept;
        };
    };
};