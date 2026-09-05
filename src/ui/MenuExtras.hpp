#pragma once

#include <Util.h>

#include <util/Base.h>

#include <gdcord/gdc.h>

#include <Geode/Geode.hpp>

namespace horrible {
    class MenuSuggest final : public geode::Popup {
    private:
        static MenuSuggest* s_inst;

        static asp::Instant s_lastSuggest;

        geode::TextInput* m_topicInput;
        geode::TextInput* m_descriptionInput;

        geode::LoadingSpinner* m_loading;

    protected:
        void onExit() override;

        void processSuggestion(geode::Button* sender);

        bool init(geode::ZStringView theme);

    public:
        static MenuSuggest* create(geode::ZStringView theme);

        static MenuSuggest* get() noexcept;
    };

    class SupporterState final : public base::Singleton<SupporterState> {
        using Callback = geode::CopyableFunction<void(geode::Result<>)>;

    private:
        bool m_supporter = false;

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
        static MenuDiscord* s_inst;

        geode::Label* m_linkLabel = nullptr;
        MenuDiscordCell* m_discordCell = nullptr;

        geode::Button* m_linkBtn = nullptr;
        geode::LoadingSpinner* m_loading = nullptr;

        ui::LabelArea* m_label = nullptr;

        void setupAuthInterface();

    protected:
        bool init(geode::ZStringView theme);

    public:
        static MenuDiscord* create(geode::ZStringView theme);

        static MenuDiscord* get() noexcept;
    };

    class MenuKofi final : public geode::Popup {
    private:
        static MenuKofi* s_inst;

    protected:
        void onExit() override;

        bool init(geode::ZStringView theme);

    public:
        static MenuKofi* create(geode::ZStringView theme);

        static MenuKofi* get() noexcept;
    };
};
