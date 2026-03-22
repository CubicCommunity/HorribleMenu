#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace horrible {
    class OptionMenuButton final : public CCLayer {
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
        void setPosition(CCPoint const& position) override;

        void setTheme(std::string theme);

        // CCLayer targeted touch listener methods
        bool ccTouchBegan(CCTouch* touch, CCEvent* event) override;
        virtual void ccTouchMoved(CCTouch* touch, CCEvent* event) override;
        virtual void ccTouchEnded(CCTouch* touch, CCEvent* event) override;

        int64_t getOpacitySetting() const noexcept;
        float getScaleSetting() const noexcept;
        bool showInLevel() const noexcept;
    };
};