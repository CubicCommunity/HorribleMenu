#pragma once

#include <Utils.h>

#include <asp/fs.hpp>

#include <Geode/Geode.hpp>

namespace horrible {
    class OptionMenuNothingNode final : public CCNode {
    protected:
        bool init(cocos2d::CCSize const& size, cocos2d::CCPoint const& pos);

    public:
        static OptionMenuNothingNode* create(cocos2d::CCSize const& size, cocos2d::CCPoint const& pos);
    };

    class OptionMenu final : public geode::Popup {
    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;

        static OptionMenu* s_inst;

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
        void setupImageBackground(asp::fs::path path);

    protected:
        OptionMenu();
        ~OptionMenu();

        void onExit() override;

        bool init() override;

    public:
        static OptionMenu* create();

        static OptionMenu* get() noexcept;
    };
};