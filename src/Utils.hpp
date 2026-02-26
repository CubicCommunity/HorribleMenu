#pragma once

#include <Horrible.hpp>

#include <classes/Jumpscares.hpp>
#include <classes/Menu.hpp>
#include <classes/Options.hpp>
#include <classes/Randng.hpp>

#include <classes/ui/MathQuiz.hpp>
#include <classes/ui/RandomAd.hpp>
#include <classes/ui/SpamChallenge.hpp>

#include <cocos2d.h>

#include <Geode/ui/Button.hpp>

#include <Geode/binding/FMODAudioEngine.hpp>

// Additional utility methods for Horrible Ideas
namespace horrible {
    // Pointer reference to this Geode mod
    inline Mod* horribleMod = geode::Mod::get();

    /**
     * Convert a chance setting number to a cooldown percentage decimal
     *
     * @param chance The chance setting number
     */
    inline float chanceToDelayPct(int chance) {
        if (chance <= 0) chance = 1;
        if (chance > 100) chance = 100;

        return ((100.f - static_cast<float>(chance)) + 1.f) / 100.f;
    };

    /**
     * Play a sound effect using FMOD
     *
     * @param name Name of the audio file
     */
    inline void playSfx(const char* file) {
        if (auto fmod = FMODAudioEngine::sharedEngine()) fmod->playEffectAsync(file);
    };

    // Get the current theme's sprites and colors
    namespace theme {
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
        inline const char* getBackgroundSprite(std::string_view theme) {
            if (theme == enums::Blue) return "GJ_square02.png";
            if (theme == enums::Green) return "GJ_square03.png";
            if (theme == enums::Dark) return "geode.loader/GE_square02.png";
            if (theme == enums::Light) return "geode.loader/white-square.png";
            if (theme == enums::Dim) return "GJ_square05.png";
            if (theme == enums::Girlypop) return "GJ_square04.png";
            if (theme == enums::Aqua) return "geode.loader/GE_square03.png";
            if (theme == enums::Geode) return "geode.loader/GE_square01.png";

            return "GJ_square01.png";
        };

        // Get the current theme's circle base color enum
        inline geode::CircleBaseColor getCircleBaseColor(std::string_view theme) {
            if (theme == enums::Blue) return geode::CircleBaseColor::Blue;
            if (theme == enums::Green) return geode::CircleBaseColor::Green;
            if (theme == enums::Dark) return geode::CircleBaseColor::Gray;
            if (theme == enums::Light) return geode::CircleBaseColor::Gray;
            if (theme == enums::Dim) return geode::CircleBaseColor::Gray;
            if (theme == enums::Girlypop) return geode::CircleBaseColor::Pink;
            if (theme == enums::Aqua) return geode::CircleBaseColor::DarkAqua;
            if (theme == enums::Geode) return geode::CircleBaseColor::DarkPurple;

            return geode::CircleBaseColor::Green;
        };

        // Get the current theme's square button sprite name
        inline const char* getButtonSquareSprite(std::string_view theme) {
            if (theme == enums::Blue) return "GJ_button_02.png";
            if (theme == enums::Green) return "GJ_button_01.png";
            if (theme == enums::Dark) return "GJ_button_05.png";
            if (theme == enums::Light) return "GJ_button_04.png";
            if (theme == enums::Dim) return "GJ_button_04.png";
            if (theme == enums::Girlypop) return "GJ_button_03.png";
            if (theme == enums::Aqua) return "geode.loader/GE_button_05.png";
            if (theme == enums::Geode) return "geode.loader/GE_button_05.png";

            return "GJ_button_01.png";
        };
    };

    namespace str = geode::utils::string; // Shortcut for geode::utils::string

    // Default option categories
    namespace category {
        inline constexpr auto playerlife = "Player Life";
        inline constexpr auto jumpscares = "Jumpscares";
        inline constexpr auto randoms = "Randoms";
        inline constexpr auto chances = "Chances";
        inline constexpr auto obstructive = "Obstructive";
        inline constexpr auto misc = "Misc";
    };

    // Fast color object references ^w^
    namespace colors {
        inline constexpr cocos2d::ccColor3B white = { 255, 255, 255 };
        inline constexpr cocos2d::ccColor3B gray = { 150, 150, 150 };
        inline constexpr cocos2d::ccColor3B red = { 225, 75, 100 };
        inline constexpr cocos2d::ccColor3B yellow = { 250, 250, 25 };
        inline constexpr cocos2d::ccColor3B green = { 100, 255, 100 };
        inline constexpr cocos2d::ccColor3B cyan = { 10, 175, 255 };
        inline constexpr cocos2d::ccColor3B black = { 0, 0, 0 };
    };

    namespace prelude {
        using namespace ::horrible;
        using namespace ::horrible::ui;
        using namespace ::horrible::util;
    };
};