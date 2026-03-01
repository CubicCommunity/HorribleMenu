#pragma once

#include <Geode/Geode.hpp>

namespace horrible {
    namespace util {
        // Get the current theme's sprites and colors
        namespace themes {
            inline constexpr auto square = "square02_001.png";

            // Theme options
            namespace enums {
                inline constexpr auto Blue = "Blue";
                inline constexpr auto Green = "Green";
                inline constexpr auto Dark = "Dark";
                inline constexpr auto Light = "Light";
                inline constexpr auto Dim = "Dim";
                inline constexpr auto Girlypop = "Girlypop";
                inline constexpr auto Aqua = "Aqua";
                inline constexpr auto Geode = "Geode";
            };

            // Get the current theme's background sprite name
            const char* getBackgroundSprite(std::string_view theme) noexcept;

            // Get the current theme's circle base color enum
            geode::CircleBaseColor getCircleBaseColor(std::string_view theme) noexcept;

            // Get the current theme's square button sprite name
            const char* getButtonSquareSprite(std::string_view theme) noexcept;
        };
    };
};