#pragma once

#include <Util.h>

#include <Geode/Geode.hpp>

namespace cs::brkd::horrible {
    class MenuNothingNode final : public cocos2d::CCNode {
    protected:
        bool init(cocos2d::CCSize const& size, cocos2d::CCPoint const& pos);

    public:
        static MenuNothingNode* create(cocos2d::CCSize const& size, cocos2d::CCPoint const& pos);
    };

    class MenuOptionCell final : public cocos2d::CCMenu {
        using Callback = geode::Function<void()>;

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;

    protected:
        MenuOptionCell();
        ~MenuOptionCell();

        bool init(cocos2d::CCSize const& size, SharedOption option, geode::ZStringView theme, bool devMode, bool hasInternet, bool view);

    public:
        static MenuOptionCell* create(cocos2d::CCSize const& size, SharedOption option, geode::ZStringView theme = "", bool devMode = false, bool hasInternet = false, bool view = false);
        static MenuOptionCell* createView(float width, SharedOption option, geode::ZStringView theme);

        void setPinCallback(Callback&& callback);

        SharedOption const& getOption() const noexcept;
        bool isCompatible() const noexcept;
    };
};