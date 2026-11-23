#pragma once

#include <Util.h>

#include <util/Base.h>

#include <Geode/Geode.hpp>

namespace cs::brkd::horrible {
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
        std::string url;
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

    protected:
        bool init(geode::ZStringView theme);

    public:
        static MenuCredits* create(geode::ZStringView theme);
    };

    class CreditsManager final : public base::Singleton<CreditsManager> {
    private:
        std::vector<LeadDevIcon> m_leadDevs;
        std::string m_license;

        geode::async::TaskHolder<geode::utils::web::WebResponse> m_creditsTask;
        geode::async::TaskHolder<geode::utils::web::WebResponse> m_licenseTask;

    public:
        void loadLeadDevs();
        void loadLicense();

        std::span<const LeadDevIcon> getLeadDevs() const noexcept;
        geode::ZStringView getLicense() const noexcept;
    };
};

template <>
struct matjson::Serialize<cs::brkd::horrible::LeadDevIcon> final {
    static geode::Result<cs::brkd::horrible::LeadDevIcon> fromJson(matjson::Value const& value);
    static matjson::Value toJson(cs::brkd::horrible::LeadDevIcon const& value);
};

template <>
struct matjson::Serialize<cs::brkd::horrible::LicenseData> final {
    static geode::Result<cs::brkd::horrible::LicenseData> fromJson(matjson::Value const& value);
    static matjson::Value toJson(cs::brkd::horrible::LicenseData const& value);
};