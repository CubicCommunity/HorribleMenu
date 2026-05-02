#pragma once

#include <Utils.h>

#include <Geode/Geode.hpp>

namespace horrible {
    class MenuOption final : public cocos2d::CCMenu {
        using Callback = Function<void()>;

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;

    protected:
        MenuOption();
        ~MenuOption();

        void onToggle(cocos2d::CCObject*);
        void onPin(cocos2d::CCObject* sender);

        bool init(cocos2d::CCSize const& size, std::weak_ptr<Option> option, geode::ZStringView theme, bool devMode, bool hasInternet);

    public:
        static MenuOption* create(cocos2d::CCSize const& size, std::weak_ptr<Option> option, geode::ZStringView theme = "", bool devMode = false, bool hasInternet = false);

        void setPinCallback(Callback&& callback) &;

        std::weak_ptr<Option> const& getOption() const noexcept;
        bool isCompatible() const noexcept;
    };
};