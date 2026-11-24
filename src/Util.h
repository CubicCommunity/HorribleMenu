#pragma once

#include <horrible/API.h>

#include <util/Include.h>
#include <util/Macros.h>

// #include <gdcord/gdc.h>

#include <cue/Util.hpp>
#include <cue/DropdownNode.hpp>
#include <cue/PlayerIcon.hpp>
#include <cue/RepeatingBackground.hpp>

#include <asp/fs.hpp>

#include <cocos2d.h>

#include <matjson.hpp>

#include <Geode/ui/Toggler.hpp>

#include <Geode/ui/GeodeUI.hpp>

#include <Geode/utils/web.hpp>
#include <Geode/utils/cocos.hpp>

// Quick utility methods for Horrible Menu
namespace cs::brkd::horrible {
    // Pointer to this Geode mod
    inline static geode::Mod* mod = geode::Mod::get();

    namespace mods {
        inline bool isImagePlus() {
            return geode::Loader::get()->isModLoaded("prevter.imageplus");
        }
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
        inline void play(geode::ZStringView file) {
            if (auto fmod = FMODAudioEngine::sharedEngine()) (void)fmod->playEffectAsync(file);
        }
    };

    namespace font {
        inline constexpr auto big = "bigFont.fnt";
        inline constexpr auto gold = "goldFont.fnt";
        inline constexpr auto chat = "chatFont.fnt";
    };

    namespace json = matjson;  // Shortcut for `matjson`
    namespace fs = asp::fs;    // Shortcut for `asp::fs`

    namespace str {
        using namespace geode::utils::string;

        inline constexpr auto isOnOff(bool on) {
            return on ? "ON" : "OFF";
        }
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

    // class SupporterState final : public base::Singleton<SupporterState> {
    //     using Callback = geode::CopyableFunction<void(geode::Result<>)>;

    // private:
    //     bool m_supporter = false;

    //     geode::async::TaskHolder<gdc::LinkResult> m_gdcTask;
    //     geode::async::TaskHolder<geode::utils::web::WebResponse> m_task;

    // public:
    //     void validateSupporter(Callback&& cb);

    //     bool isSupporter() const noexcept;
    // };

    // All namespace includes
    namespace prelude {
        using namespace ::cs::brkd::horrible;
        using namespace ::cs::brkd::horrible::ui;
        using namespace ::cs::brkd::horrible::util;
    };
};