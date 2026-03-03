#pragma once

#include <Horrible.hpp>

#include <util/Jumpscares.hpp>
#include <util/Menu.hpp>
#include <util/Options.hpp>
#include <util/Randng.hpp>
#include <util/Themes.hpp>

#include <util/ui/MathQuiz.hpp>
#include <util/ui/RandomAd.hpp>
#include <util/ui/SpamChallenge.hpp>

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
    inline float chanceToDelayPct(int chance) noexcept {
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
        if (auto fmod = FMODAudioEngine::sharedEngine()) (void)fmod->playEffectAsync(file);
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

    namespace prelude {
        using namespace ::horrible;
        using namespace ::horrible::ui;
        using namespace ::horrible::util;
    };
};