#pragma once

#include <Utils.h>

#include <Geode/Geode.hpp>

namespace horrible {
    class OptionMenuCredits final : public Popup {
    private:
        static OptionMenuCredits* s_inst;

        struct LeadDevIcon final {
            const char* name;
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