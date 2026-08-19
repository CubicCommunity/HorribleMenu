#pragma once

#include <Util.h>

#include <util/Base.h>

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

    struct DiscordLink final {
        std::string id;
        std::string username;
        std::string avatar;
    };

    class AuthState final : public base::Singleton<AuthState> {
    private:
        bool m_authorized = false;
        bool m_discordLinked = false;

        int m_accountID;
        int m_userID;
        std::string m_username;
        std::string m_token;

        DiscordLink m_discord;
        bool m_supporter = false;

    protected:
        void setAuthInfo(int accountID, int userID, std::string username, std::string token);

    public:
        void startAuth(geode::CopyableFunction<void(geode::Result<>)>&& callback);

        void setDiscordLinkInfo(DiscordLink discord);

        bool isAuthorized() const noexcept;
        bool isAuthValid() const;

        int getAccountID() const noexcept;
        int getUserID() const noexcept;
        geode::ZStringView getUsername() const noexcept;
        geode::ZStringView getToken() const noexcept;

        geode::Result<DiscordLink> getDiscord() const;
        bool isSupporter() const noexcept;
    };

    class MenuDiscordCell final : public cocos2d::CCNode {
    private:
        std::string normalizeAvatarURL(std::string url) const;

    protected:
        bool init(DiscordLink const& profile);

    public:
        static MenuDiscordCell* create(DiscordLink const& profile);
    };

    class MenuDiscord final : public geode::Popup {
    private:
        static MenuDiscord* s_inst;

        std::string m_state;
        asp::Instant m_since;
        geode::async::TaskHolder<geode::utils::web::WebResponse> m_listener;

        geode::Label* m_linkLabel = nullptr;
        MenuDiscordCell* m_discordCell = nullptr;

        geode::Button* m_linkBtn = nullptr;
        geode::LoadingSpinner* m_loading = nullptr;

        ui::LabelArea* m_label = nullptr;

        void setupAuthInterface();

    protected:
        void onExit() override;

        void checkDiscordStatus(float);

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

template <>
struct horrible::json::Serialize<horrible::DiscordLink> final {
    static geode::Result<horrible::DiscordLink> fromJson(horrible::json::Value const& value);
    static horrible::json::Value toJson(horrible::DiscordLink const& value);
};