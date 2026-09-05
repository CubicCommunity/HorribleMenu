#pragma once

#include <Util.h>

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
            std::string sprite;
            std::string id;
            geode::Button::ButtonCallback callback;
        };

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;

        static Menu* s_inst;

        void setupSafeModeNode(bool safeMode);
        void setupImageBackground(fs::path const& path);
        void setupSillyFilterDropdown(cocos2d::CCPoint const& pos);

    protected:
        Menu();
        ~Menu();

        void onEnter() override;
        void onExit() override;

        bool init() override;

    public:
        static Menu* create();

        static Menu* get() noexcept;
    };

    // For convenience
    namespace menu {
        // Open the Horrible Menu mod option menu
        inline void open(bool force = false) {
            if (auto old = Menu::get()) {
                cue::resetNode(old);
                if (!force) return;
            };

            if (auto popup = Menu::create()) popup->show();
        };
    };
};