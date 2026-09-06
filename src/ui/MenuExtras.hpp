#pragma once

#include <Util.h>

#include <util/Base.h>

#include <gdcord/gdc.h>

#include <Geode/Geode.hpp>

namespace horrible {
    class MenuSuggest final : public geode::Popup {
    private:
        static asp::Instant s_lastSuggest;

        geode::TextInput* m_topicInput;
        geode::TextInput* m_descriptionInput;

        geode::LoadingSpinner* m_loading;

        geode::async::TaskHolder<geode::Result<std::string>> m_authTask;
        geode::async::TaskHolder<geode::utils::web::WebResponse> m_sendTask;

    protected:
        void processSuggestion(geode::Button* sender);

        bool init(geode::ZStringView theme);

    public:
        static MenuSuggest* create(geode::ZStringView theme);
    };

    class SupporterState final : public base::Singleton<SupporterState> {
        using Callback = geode::CopyableFunction<void(geode::Result<>)>;

    private:
        bool m_supporter = false;

        geode::async::TaskHolder<geode::utils::web::WebResponse> m_task;

    public:
        void validateSupporter(Callback&& cb);

        bool isSupporter() const noexcept;
    };

    class MenuDiscordCell final : public cocos2d::CCNode {
    private:
        std::string normalizeAvatarURL(std::string url) const;

    protected:
        bool init(gdc::DiscordLink const& profile);

    public:
        static MenuDiscordCell* create(gdc::DiscordLink const& profile);
    };

    class MenuDiscord final : public geode::Popup {
    private:
        geode::Label* m_linkLabel = nullptr;
        MenuDiscordCell* m_discordCell = nullptr;

        geode::Button* m_linkBtn = nullptr;
        geode::LoadingSpinner* m_loading = nullptr;

        ui::LabelArea* m_label = nullptr;

        void setupAuthInterface(bool forceHide = false);

    protected:
        bool init(geode::ZStringView theme);

    public:
        static MenuDiscord* create(geode::ZStringView theme);
    };

    class MenuKofi final : public geode::Popup {
    private:
        ui::LabelArea* m_infoContainer = nullptr;
        geode::LoadingSpinner* m_loading = nullptr;

        geode::Button* m_linkBtn = nullptr;
        geode::Label* m_linkLabel = nullptr;
        geode::LoadingSpinner* m_linkLoading = nullptr;

    protected:
        bool init(geode::ZStringView theme);

    public:
        static MenuKofi* create(geode::ZStringView theme);
    };
};
