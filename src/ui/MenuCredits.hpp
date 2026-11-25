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

        geode::async::TaskHolder<geode::utils::web::WebResponse> m_creditsTask;

    public:
        void loadLeadDevs();

        std::span<const LeadDevIcon> getLeadDevs() const noexcept;
    };
};

template <>
struct matjson::Serialize<cs::brkd::horrible::LeadDevIcon> final {
    static geode::Result<cs::brkd::horrible::LeadDevIcon> fromJson(Value const& value);
    static matjson::Value toJson(cs::brkd::horrible::LeadDevIcon const& value);
};