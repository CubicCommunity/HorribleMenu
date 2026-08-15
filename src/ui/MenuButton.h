#pragma once

#include <Util.h>

#include <Geode/Geode.hpp>

template <>
struct horrible::json::Serialize<cocos2d::CCPoint> final {
    static geode::Result<cocos2d::CCPoint> fromJson(horrible::json::Value const& value);
    static horrible::json::Value toJson(cocos2d::CCPoint const& value);
};

namespace horrible {
    class MenuButton final : public cocos2d::CCLayer {
    private:
        struct Impl;
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
        void setShowInEditor(bool show);

        void setTheme(std::string theme);
        void setButtonIcon(std::string icon);

        void setScale(float scale) override;

        uint8_t getOpacitySetting() const noexcept;
        float getScaleSetting() const noexcept;

        bool showInLevel() const noexcept;
        bool showInEditor() const noexcept;
    };
};