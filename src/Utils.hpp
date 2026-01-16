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

    namespace str = geode::utils::string; // Shortcut for geode::utils::string

    // Default option IDs
    namespace key {
        inline constexpr auto oxygen = "oxygen";
        inline constexpr auto health = "health";
        inline constexpr auto grief = "grief";
        inline constexpr auto congregation = "congregation";
        inline constexpr auto math_quiz = "math_quiz";
        inline constexpr auto mock = "mock";
        inline constexpr auto freeze = "freeze";
        inline constexpr auto spam = "spam";
        inline constexpr auto achieve = "achieve";
        inline constexpr auto crash_death = "crash_death";
        inline constexpr auto confetti = "confetti";
        inline constexpr auto no_jump = "no_jump";
        inline constexpr auto gravity = "gravity";
        inline constexpr auto death = "death";
        inline constexpr auto upside_down = "upside_down";
        inline constexpr auto ads = "ads";
        inline constexpr auto black_screen = "black_screen";
        inline constexpr auto parry = "parry";
        inline constexpr auto double_jump = "double_jump";
        inline constexpr auto sleepy = "sleepy";
        inline constexpr auto pauses = "pauses";
        inline constexpr auto ice_level = "ice_level";
        inline constexpr auto random_mirror = "random_mirror";
        inline constexpr auto random_speed = "random_speed";
        inline constexpr auto random_icon = "random_icon";
        inline constexpr auto blinking_icon = "blinking_icon";
        inline constexpr auto dementia = "dementia";
        inline constexpr auto earthquake = "earthquake";
        inline constexpr auto fake_crash = "fake_crash";
        inline constexpr auto gambler = "gambler";
        inline constexpr auto placebo = "placebo";
        inline constexpr auto click_speed = "click_speed";
        inline constexpr auto motivation = "motivation";
        inline constexpr auto flipped = "flipped";
        inline constexpr auto friends = "friends";
        inline constexpr auto size_changer = "size_changer";
        inline constexpr auto timewarp_jump = "timewarp_jump";
    };

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
        inline constexpr cocos2d::ccColor3B red = { 225, 75, 75 };
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