#pragma once

#include <ranges>

#include <Horrible.hpp>

#include <classes/Jumpscares.hpp>
#include <classes/Menu.hpp>
#include <classes/Options.hpp>
#include <classes/Randng.hpp>

#include <classes/ui/MathQuiz.hpp>
#include <classes/ui/RandomAd.hpp>
#include <classes/ui/SpamChallenge.hpp>

#include <cocos2d.h>

#include <Geode/utils/ZStringView.hpp>

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

    template <typename Map>
    inline void delegateHooks(geode::ZStringView id, Map& hooks) {
        if (auto om = OptionManager::get()) {
            auto value = om->getOption(id);

            std::vector<geode::Hook*> allHooks;
            for (auto& hook : hooks | std::views::values) {
                hook->setAutoEnable(value);
                allHooks.push_back(hook.get());
            };

            om->addDelegate(
                id,
                [allHooks = std::move(allHooks), id, om] {
                    auto value = om->getOption(id);
                    for (auto hook : allHooks) (void)hook->toggle(value);
                }
            );
        };
    };
};

#define DELEGATE_HOOKS(id) \
static void onModify(auto& self) { \
    horrible::delegateHooks(id, self.m_hooks); \
}