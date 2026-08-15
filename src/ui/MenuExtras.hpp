#pragma once

#include <Util.h>

#include <util/Base.h>

#include <Geode/Geode.hpp>

namespace horrible {
    class MenuSuggest final : public geode::Popup {
    private:
        static MenuSuggest* s_inst;

        geode::TextInput* m_topicInput;
        geode::TextInput* m_descriptionInput;

    protected:
        void onExit() override;

        bool init(geode::ZStringView theme);

    public:
        static MenuSuggest* create(geode::ZStringView theme);

        static MenuSuggest* get() noexcept;
    };

    class AuthState final : public base::Singleton<AuthState> {
    private:
        int m_accountID;
        int m_userID;
        std::string m_username;
        std::string m_token;

    public:
        void setAuthInfo(int accountID, int userID, std::string username, std::string token);

        int getAccountID() const noexcept;
        int getUserID() const noexcept;
        geode::ZStringView getUsername() const noexcept;
        geode::ZStringView getToken() const noexcept;
    };

    class MenuDiscord final : public geode::Popup {
    private:
        static MenuDiscord* s_inst;

    protected:
        void onExit() override;

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