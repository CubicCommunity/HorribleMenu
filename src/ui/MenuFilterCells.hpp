#pragma once

#include <Utils.h>

#include <Geode/Geode.hpp>

namespace horrible {
    class MenuCategoryFilterCell final : public cocos2d::CCMenu {
        using Callback = geode::Function<void(std::string_view, bool)>;

    private:
        std::string m_category = "";             // The category name
        CCMenuItemToggler* m_toggler = nullptr;  // The toggler for the option
        Callback m_toggleCallback = nullptr;     // Callback for when the category is toggled

        geode::NineSlice* m_bg = nullptr;

    protected:
        void onToggle(CCObject* sender);

        bool init(cocos2d::CCSize const& size, std::string category);

    public:
        static MenuCategoryFilterCell* create(cocos2d::CCSize const& size, std::string category);

        void setToggleCallback(Callback&& callback);
        void setToggled(bool on);

        geode::ZStringView getCategory() const noexcept;
    };

    class MenuSillyFilterCell final : public cocos2d::CCNode {
    private:
        SillyTier m_silly = SillyTier::None;

    protected:
        bool init(cocos2d::CCSize const& size, SillyTier silly, std::string id, geode::ZStringView label, cocos2d::ccColor3B const& color);

    public:
        static MenuSillyFilterCell* create(cocos2d::CCSize const& size, SillyTier silly, std::string id, geode::ZStringView label, cocos2d::ccColor3B const& color);

        SillyTier getSillyTier() const noexcept;
    };
};