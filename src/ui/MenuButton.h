#pragma once

#include <Geode/Geode.hpp>

template <>
struct matjson::Serialize<cocos2d::CCPoint> final {
    static geode::Result<cocos2d::CCPoint> fromJson(matjson::Value const& value);
    static matjson::Value toJson(cocos2d::CCPoint const& value);
};

namespace horrible {
    class MenuButton final : public cocos2d::CCLayer {
    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;

        void setupSprite();

    protected:
        MenuButton();
        ~MenuButton();

        void onScaleEnd();

        void onEnter() override;

        bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
        void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
        void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;

        bool init() override;

        static MenuButton* create();

    public:
        static MenuButton* get();

        void setOpacity(GLubyte opacity);
        void setShowInLevel(bool show);

        void setScale(float scale) override;

        void setTheme(std::string theme);
        void setButtonIcon(std::string icon);

        uint8_t getOpacitySetting() const noexcept;
        float getScaleSetting() const noexcept;
        bool showInLevel() const noexcept;
    };
};