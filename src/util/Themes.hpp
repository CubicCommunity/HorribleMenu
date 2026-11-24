#pragma once

#include <rhatt/ui/consts.hpp>

#include <rhatt/util/colors.hpp>
#include <rhatt/util/strings.hpp>

#include <cocos2d.h>

#include <Geode/Geode.hpp>

namespace cs::brkd::horrible {
    namespace util {
        // Get the current theme's sprites and colors
        namespace themes {
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
                if (theme == enums::Dark) return "GE_square02.png"_gode;
                if (theme == enums::Light) return "white-square.png"_gode;
                if (theme == enums::Dim) return "GJ_square05.png";
                if (theme == enums::Girlypop) return "GJ_square04.png";
                if (theme == enums::Aqua) return "GE_square03.png"_gode;
                if (theme == enums::Geode) return "GE_square01.png"_gode;

                return "GJ_square01.png";
            }

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
            }

            // Get the current theme's square button sprite name
            inline constexpr auto getButtonSquareSprite(std::string_view theme) noexcept {
                if (theme == enums::Icy) return "GJ_button_02.png";
                if (theme == enums::Grass) return "GJ_button_01.png";
                if (theme == enums::Dark) return "GJ_button_05.png";
                if (theme == enums::Light) return "GJ_button_04.png";
                if (theme == enums::Dim) return "GJ_button_04.png";
                if (theme == enums::Girlypop) return "GJ_button_03.png";
                if (theme == enums::Aqua) return "GJ_button_05.png";
                if (theme == enums::Geode) return "GE_button_05.png"_gode;

                return "GJ_button_01.png";
            }

            // Get the current theme's color
            inline constexpr auto getColor(std::string_view theme) noexcept {
                if (theme == enums::Icy) return cw::colors::blue;
                if (theme == enums::Grass) return cw::colors::green;
                if (theme == enums::Dark) return cw::colors::white;
                if (theme == enums::Light) return cw::colors::white;
                if (theme == enums::Dim) return cw::colors::gray;
                if (theme == enums::Girlypop) return cw::colors::pink;
                if (theme == enums::Aqua) return cw::colors::cyan;
                if (theme == enums::Geode) return cw::colors::gold;

                return cw::colors::white;
            }

            inline constexpr auto createThemeCircleSprite(geode::CircleBaseColor btns) {
                return geode::CircleButtonSprite::createWithSpriteFrameName(cw::ui::sprites::close, 0.875f, btns, geode::CircleBaseSize::Small);
            }

            // Floating button icon options
            namespace icons {
                inline constexpr auto Default = "Default";
                inline constexpr auto Classic = "Classic";
                inline constexpr auto Pride = "Pride";
                inline constexpr auto TransPride = "Trans Pride";
                inline constexpr auto Breakeode = "Breakeode";
                inline constexpr auto SteamHappy = "Steam Happy";
                inline constexpr auto SoggyCat = "Soggy Cat";
                inline constexpr auto Erymanthus = "erymanthus";
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
                if (icon == icons::Erymanthus) return "icon_eri.png"_spr;
                if (icon == icons::CryingSkull) return "icon_skull.png"_spr;
                if (icon == icons::TheYellowOne) return "icon_yellow.png"_spr;
                if (icon == icons::ColonThree) return "icon_colonthree.png"_spr;
                if (icon == icons::Durr) return "icon_durr.png"_spr;
                if (icon == icons::SapphireSDK) return "icon_sapphire.png"_spr;

                return "icon.png"_spr;
            }
        };
    };
};