#pragma once

#include <Utils.h>

#include <Geode/Geode.hpp>

namespace horrible {
    class Menu final : public geode::Popup {
        struct TierFilterBtnData final {
            SillyTier tier;
            const char* label;
            const char* id;
            cocos2d::ccColor3B const& color;
        };

        struct SocialBtnData final {
            const char* sprite;
            const char* id;
            Button::ButtonCallback callback;
            float scale = 0.75f;
        };

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;

        static Menu* s_inst;

        void setupSafeModeNode(bool safeMode);
        void setupImageBackground(fs::path const& path);

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