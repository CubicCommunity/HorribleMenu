#pragma once

#include <Geode/Geode.hpp>

namespace horrible {
    class MenuOptionCategory final : public cocos2d::CCMenu {
        using Callback = geode::Function<void(std::string_view, bool)>;

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;

    protected:
        MenuOptionCategory();
        ~MenuOptionCategory();

        void onToggle(CCObject* sender);

        bool init(cocos2d::CCSize const& size, std::string category);

    public:
        static MenuOptionCategory* create(cocos2d::CCSize const& size, std::string category);

        void setToggleCallback(Callback&& callback);
        void setToggled(bool on);

        geode::ZStringView getCategory() const noexcept;
    };
};