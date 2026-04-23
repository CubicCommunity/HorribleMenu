#pragma once

#include <Utils.h>

#include <Geode/Geode.hpp>

namespace horrible {
    class MenuNothingNode final : public CCNode {
    protected:
        bool init(cocos2d::CCSize const& size, cocos2d::CCPoint const& pos);

    public:
        static MenuNothingNode* create(cocos2d::CCSize const& size, cocos2d::CCPoint const& pos);
    };

    class Menu final : public geode::Popup {
    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;

        static Menu* s_inst;

        struct TierFilterBtnData final {
            SillyTier tier;
            const char* label;
            const char* id;
            ccColor3B color;
        };

        struct SocialBtnData final {
            const char* sprite;
            const char* id;
            Button::ButtonCallback callback;
            float scale = 0.75f;
        };

        void setupSafeModeNode(bool safeMode);
        void setupImageBackground(std::filesystem::path path);

    protected:
        Menu();
        ~Menu();

        void onExit() override;

        bool init() override;

    public:
        static Menu* create();

        static Menu* get() noexcept;
    };
};