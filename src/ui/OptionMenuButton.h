#pragma once

#include <Geode/Geode.hpp>

namespace horrible {
    class OptionMenuButton final : public cocos2d::CCLayer {
    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;

        void setupSprite();

    protected:
        OptionMenuButton();
        ~OptionMenuButton();

        void onScaleEnd();

        virtual bool init() override;

        static OptionMenuButton* create();

    public:
        static OptionMenuButton* get() noexcept;

        void setOpacity(GLubyte opacity);
        void setShowInLevel(bool show);

        void onEnter() override;

        void setScale(float scale) override;
        void setPosition(cocos2d::CCPoint const& position) override;

        void setTheme(std::string theme);
        void setButtonIcon(std::string icon);

        // CCLayer targeted touch listener methods
        bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
        virtual void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
        virtual void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;

        int64_t getOpacitySetting() const noexcept;
        float getScaleSetting() const noexcept;
        bool showInLevel() const noexcept;
    };
};