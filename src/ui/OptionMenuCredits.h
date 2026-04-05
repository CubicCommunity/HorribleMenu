#pragma once

#include <Utils.h>

#include <Geode/Geode.hpp>

namespace horrible {
    class OptionMenuPlayer final : public cocos2d::CCNode {
    protected:
        bool init(geode::ZStringView name, int account, int icon, ccColor3B const& color1, ccColor3B const& color2, ccColor3B const& glowColor);

    public:
        static OptionMenuPlayer* create(geode::ZStringView name, int account, int icon, ccColor3B const& color1, ccColor3B const& color2, ccColor3B const& glowColor);
    };

    class OptionMenuCredits final : public Popup, private FLAlertLayerProtocol {
    private:
        static OptionMenuCredits* s_inst;

        struct LeadDevIcon final {
            const char* id;
            const char* name;
            int account;
            int icon;
            ccColor3B color1;
            ccColor3B color2;
            ccColor3B glowColor;
        };

    protected:
        void onClose(CCObject* sender) override;
        void onExit() override;
        void cleanup() override;

        bool init(geode::ZStringView theme);

    public:
        static OptionMenuCredits* create(geode::ZStringView theme);

        static OptionMenuCredits* get() noexcept;
    };
};