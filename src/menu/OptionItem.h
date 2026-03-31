#pragma once

#include <Utils.h>

#include <Geode/Geode.hpp>

namespace horrible {
    class OptionItem final : public cocos2d::CCMenu, private FLAlertLayerProtocol {
    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;

        using Callback = Function<void()>;

    protected:
        OptionItem();
        ~OptionItem();

        void onToggle(cocos2d::CCObject*);
        void onPin(cocos2d::CCObject* sender);

        bool init(cocos2d::CCSize const& size, Option option, bool devMode);

    public:
        static OptionItem* create(cocos2d::CCSize const& size, Option option, bool devMode);

        void setPinCallback(Callback&& callback);

        Option const& getOption() const noexcept;
        bool isCompatible() const noexcept;
    };
};