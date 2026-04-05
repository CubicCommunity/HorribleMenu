#pragma once

#include <Geode/Geode.hpp>

namespace horrible {
    class OptionCategoryItem final : public cocos2d::CCMenu {
    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;

        using Callback = geode::Function<void(std::string_view, bool)>;

    protected:
        OptionCategoryItem();
        ~OptionCategoryItem();

        void onToggle(CCObject* sender);

        bool init(cocos2d::CCSize const& size, std::string category);

    public:
        static OptionCategoryItem* create(cocos2d::CCSize const& size, std::string category);

        void setToggleCallback(Callback&& callback);
        void setToggled(bool on);

        geode::ZStringView getCategory() const noexcept;
    };
};