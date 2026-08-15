#pragma once

#include <cocos2d.h>

#include <Geode/Geode.hpp>

namespace horrible {
    // Fast color object references ^w^
    namespace colors {
        inline constexpr cocos2d::ccColor3B white = {255, 255, 255};
        inline constexpr cocos2d::ccColor3B gray = {150, 150, 150};
        inline constexpr cocos2d::ccColor3B red = {225, 75, 100};
        inline constexpr cocos2d::ccColor3B yellow = {250, 250, 25};
        inline constexpr cocos2d::ccColor3B green = {100, 255, 100};
        inline constexpr cocos2d::ccColor3B cyan = {10, 175, 255};
        inline constexpr cocos2d::ccColor3B blue = {0, 122, 255};
        inline constexpr cocos2d::ccColor3B pink = {255, 105, 180};
        inline constexpr cocos2d::ccColor3B gold = {255, 200, 150};
        inline constexpr cocos2d::ccColor3B black = {0, 0, 0};

        namespace internal {

            inline constexpr cocos2d::ccColor3B lerpColor(cocos2d::ccColor3B const& a, cocos2d::ccColor3B const& b, float t) noexcept {
                return {
                    static_cast<GLubyte>(a.r + (b.r - a.r) * t),
                    static_cast<GLubyte>(a.g + (b.g - a.g) * t),
                    static_cast<GLubyte>(a.b + (b.b - a.b) * t),
                };
            };
        };

        inline constexpr cocos2d::ccColor3B fadeColor(float value) noexcept {
            if (value > 100.f) value = 100.f;
            if (value < 0.f) value = 0.f;

            if (value <= 50.0f) {
                auto t = value / 50.0f;
                return internal::lerpColor(red, yellow, t);
            } else {
                auto t = (value - 50.0f) / 50.0f;
                return internal::lerpColor(yellow, green, t);
            };
        };
    };

    namespace util {
        // Get the current theme's sprites and colors
        namespace themes {
            inline constexpr auto square = "geode.loader/white-square.png";
            inline constexpr auto close = "geode.loader/close.png";

            // Theme options
            namespace enums {
                inline constexpr auto Icy = "Icy";
                inline constexpr auto Grass = "Grass";
                inline constexpr auto Dark = "Dark";
                inline constexpr auto Light = "Light";
                inline constexpr auto Dim = "Dim";
                inline constexpr auto Girlypop = "Girlypop";
                inline constexpr auto Aqua = "Aqua";
                inline constexpr auto Geode = "Geode";
            };

            // Get the current theme's background sprite name
            inline constexpr auto getBackgroundSprite(std::string_view theme) noexcept {
                if (theme == enums::Icy) return "GJ_square02.png";
                if (theme == enums::Grass) return "GJ_square03.png";
                if (theme == enums::Dark) return "geode.loader/GE_square02.png";
                if (theme == enums::Light) return "geode.loader/white-square.png";
                if (theme == enums::Dim) return "GJ_square05.png";
                if (theme == enums::Girlypop) return "GJ_square04.png";
                if (theme == enums::Aqua) return "geode.loader/GE_square03.png";
                if (theme == enums::Geode) return "geode.loader/GE_square01.png";

                return "GJ_square01.png";
            };

            // Get the current theme's circle base color enum
            inline constexpr auto getCircleBaseColor(std::string_view theme) noexcept {
                if (theme == enums::Icy) return geode::CircleBaseColor::Blue;
                if (theme == enums::Grass) return geode::CircleBaseColor::Green;
                if (theme == enums::Dark) return geode::CircleBaseColor::Gray;
                if (theme == enums::Light) return geode::CircleBaseColor::Gray;
                if (theme == enums::Dim) return geode::CircleBaseColor::Gray;
                if (theme == enums::Girlypop) return geode::CircleBaseColor::Pink;
                if (theme == enums::Aqua) return geode::CircleBaseColor::DarkAqua;
                if (theme == enums::Geode) return geode::CircleBaseColor::DarkPurple;

                return geode::CircleBaseColor::Green;
            };

            // Get the current theme's square button sprite name
            inline constexpr auto getButtonSquareSprite(std::string_view theme) noexcept {
                if (theme == enums::Icy) return "GJ_button_02.png";
                if (theme == enums::Grass) return "GJ_button_01.png";
                if (theme == enums::Dark) return "GJ_button_05.png";
                if (theme == enums::Light) return "GJ_button_04.png";
                if (theme == enums::Dim) return "GJ_button_04.png";
                if (theme == enums::Girlypop) return "GJ_button_03.png";
                if (theme == enums::Aqua) return "GJ_button_05.png";
                if (theme == enums::Geode) return "geode.loader/GE_button_05.png";

                return "GJ_button_01.png";
            };

            // Get the current theme's color
            inline constexpr auto getColor(std::string_view theme) noexcept {
                if (theme == enums::Icy) return colors::blue;
                if (theme == enums::Grass) return colors::green;
                if (theme == enums::Dark) return colors::white;
                if (theme == enums::Light) return colors::white;
                if (theme == enums::Dim) return colors::gray;
                if (theme == enums::Girlypop) return colors::pink;
                if (theme == enums::Aqua) return colors::cyan;
                if (theme == enums::Geode) return colors::gold;

                return colors::white;
            };

            inline constexpr auto createThemeCircleSprite(geode::CircleBaseColor btns) {
                return geode::CircleButtonSprite::createWithSpriteFrameName(themes::close, 0.875f, btns, geode::CircleBaseSize::Small);
            };

            // Floating button icon options
            namespace icons {
                inline constexpr auto Default = "Default";
                inline constexpr auto Classic = "Classic";
                inline constexpr auto Pride = "Pride";
                inline constexpr auto TransPride = "Trans Pride";
                inline constexpr auto Breakeode = "Breakeode";
                inline constexpr auto SteamHappy = "Steam Happy";
                inline constexpr auto SoggyCat = "Soggy Cat";
                inline constexpr auto CryingSkull = "Crying Skull";
                inline constexpr auto TheYellowOne = "The Yellow One";
                inline constexpr auto ColonThree = ":3";
                inline constexpr auto Durr = "durr";
                inline constexpr auto SapphireSDK = "Sapphire SDK";
            };

            // Get the current icon's sprite name
            inline constexpr auto getIconSprite(std::string_view icon) noexcept {
                if (icon == icons::Default) return "icon.png"_spr;
                if (icon == icons::Classic) return "icon_classic.png"_spr;
                if (icon == icons::Pride) return "icon_pride.png"_spr;
                if (icon == icons::TransPride) return "icon_transpride.png"_spr;
                if (icon == icons::Breakeode) return "icon_breakeode.png"_spr;
                if (icon == icons::SteamHappy) return "icon_steam.png"_spr;
                if (icon == icons::SoggyCat) return "icon_soggy.png"_spr;
                if (icon == icons::CryingSkull) return "icon_skull.png"_spr;
                if (icon == icons::TheYellowOne) return "icon_yellow.png"_spr;
                if (icon == icons::ColonThree) return "icon_colonthree.png"_spr;
                if (icon == icons::Durr) return "icon_durr.png"_spr;
                if (icon == icons::SapphireSDK) return "icon_sapphire.png"_spr;

                return "icon.png"_spr;
            };
        };
    };
};