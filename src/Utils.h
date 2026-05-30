#pragma once

#include <horrible/API.h>
#include <horrible/OptionalAPI.hpp>

#include <util/Cursor.hpp>
#include <util/Jumpscares.hpp>
#include <util/Options.hpp>
#include <util/Random.hpp>
#include <util/Themes.hpp>

#include <util/ui/Captcha.hpp>
#include <util/ui/MathQuiz.hpp>
#include <util/ui/RandomAd.hpp>
#include <util/ui/SpamChallenge.hpp>
#include <util/ui/TermsAndConditions.hpp>
#include <util/ui/WhackButton.hpp>

#include <cue/Util.hpp>
#include <cue/DropdownNode.hpp>
#include <cue/PlayerIcon.hpp>

#include <asp/fs.hpp>

#include <cocos2d.h>

#include <Geode/ui/Button.hpp>

#include <Geode/utils/cocos.hpp>

#include <Geode/binding/FMODAudioEngine.hpp>

#define HIGHEST_Z cocos2d::CCScene::get()->getHighestChildZ() + 1

// Additional utility methods for Horrible Menu
namespace horrible {
    // Pointer to this Geode mod
    inline static Mod* mod = geode::Mod::get();

    /**
     * Convert a chance setting number to a cooldown percentage decimal
     *
     * @param chance The chance setting number
     */
    inline constexpr float chanceToDelayPct(uint8_t chance = 50) noexcept {
        if (chance <= 0) chance = 0;
        if (chance > 100) chance = 100;

        return 1.f - (static_cast<float>(chance) / 100.f);
    };

    // For convenience
    namespace sfx {
        namespace file {
            inline constexpr auto pop = "chest07.ogg";
            inline constexpr auto good = "crystal01.ogg";
            inline constexpr auto bad = "explode_11.ogg";

            inline constexpr auto click = "chestClick.ogg";
            inline constexpr auto count = "counter003.ogg";
        };

        /**
         * Play a sound effect using FMOD
         *
         * @param name Name of the audio file
         */
        inline void play(const char* file) {
            if (auto fmod = FMODAudioEngine::sharedEngine()) (void)fmod->playEffectAsync(file);
        };
    };

    namespace font {
        inline constexpr auto big = "bigFont.fnt";
        inline constexpr auto gold = "goldFont.fnt";
        inline constexpr auto chat = "chatFont.fnt";
    };

    namespace fs = asp::fs;                // Shortcut for `asp::fs`
    namespace str = geode::utils::string;  // Shortcut for `geode::utils::string`

    namespace popup {
        inline void closeBtnID(CCMenuItemSpriteExtra* btn) {
            if (btn) btn->setID("close-btn");  // xd
        };
    };

    // For convenience
    namespace setting {
        inline constexpr auto SafeMode = "safe-mode";
        inline constexpr auto DynamicSafeMode = "dyn-safe-mode";
        inline constexpr auto FloatingBtn = "floating-btn";
    };

    // Default option categories
    namespace category {
        inline constexpr auto playerlife = "Player Life";
        inline constexpr auto jumpscares = "Jumpscares";
        inline constexpr auto mechanics = "Mechanics";
        inline constexpr auto randoms = "Randoms";
        inline constexpr auto chances = "Chances";
        inline constexpr auto obstructive = "Obstructive";
        inline constexpr auto misc = "Misc";
    };

    // got tired of typing {0.5, 0.5} a billion times
    namespace anchor {
        inline constexpr cocos2d::CCPoint center = {0.5f, 0.5f};
    };

    // All namespace includes
    namespace prelude {
        using namespace ::horrible;
        using namespace ::horrible::ui;
        using namespace ::horrible::util;
    };
};