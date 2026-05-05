#pragma once

#include <Utils.h>

#include <Geode/Geode.hpp>

namespace horrible {
    class MenuNothingNode final : public cocos2d::CCNode {
    protected:
        bool init(cocos2d::CCSize const& size, cocos2d::CCPoint const& pos);

    public:
        static MenuNothingNode* create(cocos2d::CCSize const& size, cocos2d::CCPoint const& pos);
    };

    class MenuOptionCell final : public cocos2d::CCMenu {
        using Callback = geode::Function<void()>;

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;

    protected:
        MenuOptionCell();
        ~MenuOptionCell();

        void onToggle(cocos2d::CCObject*);
        void onPin(cocos2d::CCObject* sender);

        bool init(cocos2d::CCSize const& size, std::weak_ptr<Option> option, geode::ZStringView theme, bool devMode, bool hasInternet);

    public:
        static MenuOptionCell* create(cocos2d::CCSize const& size, std::weak_ptr<Option> option, geode::ZStringView theme = "", bool devMode = false, bool hasInternet = false);

        void setPinCallback(Callback&& callback);

        std::weak_ptr<Option> const& getOption() const noexcept;
        bool isCompatible() const noexcept;
    };
};