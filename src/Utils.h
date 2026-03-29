#pragma once

#include <Horrible.h>

#include <util/Cursor.hpp>
#include <util/Jumpscares.hpp>
#include <util/Menu.hpp>
#include <util/Options.hpp>
#include <util/Randng.hpp>
#include <util/Themes.hpp>

#include <util/ui/MathQuiz.hpp>
#include <util/ui/RandomAd.hpp>
#include <util/ui/SpamChallenge.hpp>
#include <util/ui/WhackButton.hpp>

#include <cocos2d.h>

#include <Geode/ui/Button.hpp>

#include <Geode/binding/FMODAudioEngine.hpp>

// Additional utility methods for Horrible Menu
namespace horrible {
    // Pointer to this Geode mod
    inline Mod* thisMod = geode::Mod::get();

    /**
     * Convert a chance setting number to a cooldown percentage decimal
     *
     * @param chance The chance setting number
     */
    inline constexpr float chanceToDelayPct(int chance = 50) noexcept {
        if (chance <= 0) chance = 1;
        if (chance > 100) chance = 100;

        return ((100.f - static_cast<float>(chance)) + 1.f) / 100.f;
    };

    // For convenience
    namespace sfx {
        /**
         * Play a sound effect using FMOD
         *
         * @param name Name of the audio file
         */
        inline void play(const char* file) {
            if (auto fmod = FMODAudioEngine::sharedEngine()) (void)fmod->playEffectAsync(file);
        };
    };

    namespace str = geode::utils::string;  // Shortcut for geode::utils::string

    // For convenience
    namespace setting {
        inline constexpr auto SafeMode = "safe-mode";
        inline constexpr auto FloatingBtn = "floating-btn";
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

    // All namespace includes
    namespace prelude {
        using namespace ::horrible;
        using namespace ::horrible::ui;
        using namespace ::horrible::util;
    };
};