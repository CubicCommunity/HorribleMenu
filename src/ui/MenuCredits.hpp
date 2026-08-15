#pragma once

#include <Util.h>

#include <util/Base.h>

#include <Geode/Geode.hpp>

namespace horrible {
    struct LeadDevIcon final {
        std::string id;
        std::string name;
        int account;
        int icon;
        int color1;
        int color2;
        int glowColor;
    };

    struct LicenseData final {
        std::string key;
        std::string name;
        std::string spdxID;
        std::string url;
        std::string nodeID;
    };

    class MenuPlayer final : public cocos2d::CCNode {
    protected:
        bool init(geode::ZStringView name, int account, int icon, int color1, int color2, int glowColor);

    public:
        static MenuPlayer* create(geode::ZStringView name, int account, int icon, int color1, int color2, int glowColor);
    };

    class MenuCredits final : public geode::Popup {
        struct ResourceButton final {
            std::string id;
            std::string label;
            geode::Button::ButtonCallback callback;
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

    class CreditsManager final : public base::Singleton<CreditsManager> {
    private:
        std::vector<LeadDevIcon> m_leadDevs;
        std::string m_license;

    public:
        void loadLeadDevs();
        void loadLicense();

        std::span<const LeadDevIcon> getLeadDevs() const noexcept;
        geode::ZStringView getLicense() const noexcept;
    };
};

template <>
struct horrible::json::Serialize<horrible::LeadDevIcon> final {
    static geode::Result<horrible::LeadDevIcon> fromJson(horrible::json::Value const& value);
    static horrible::json::Value toJson(horrible::LeadDevIcon const& value);
};

template <>
struct horrible::json::Serialize<horrible::LicenseData> final {
    static geode::Result<horrible::LicenseData> fromJson(horrible::json::Value const& value);
    static horrible::json::Value toJson(horrible::LicenseData const& value);
};