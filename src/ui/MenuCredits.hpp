#pragma once

#include <Utils.h>

#include <Geode/Geode.hpp>

namespace horrible {
    class MenuPlayer final : public cocos2d::CCNode {
    protected:
        bool init(geode::ZStringView name, int account, int icon, int color1, int color2, int glowColor);

    public:
        static MenuPlayer* create(geode::ZStringView name, int account, int icon, int color1, int color2, int glowColor);
    };

    class MenuCredits final : public geode::Popup {
        struct LeadDevIcon final {
            const char* id;
            const char* name;
            int account;
            int icon;
            int color1;
            int color2;
            int glowColor;
        };

    private:
        static MenuCredits* s_inst;

    protected:
        void onExit() override;

        bool init(geode::ZStringView theme);

    public:
        static MenuCredits* create(geode::ZStringView theme);

        static MenuCredits* get() noexcept;
    };
};