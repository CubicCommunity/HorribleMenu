#pragma once

#include <Util.h>

#include <Geode/Geode.hpp>

namespace cs::brkd::horrible {
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
        void setupImageBackground(cw::fs::PathView path);
        void setupSillyFilterDropdown(cocos2d::CCPoint const& pos);

        void showAnimation();

    protected:
        Menu();
        ~Menu();

        void onEnter() override;
        void onExit() override;

        bool init(std::optional<std::string> theme);

    public:
        static Menu* create(std::optional<std::string> theme = std::nullopt);

        static Menu*& get() noexcept;
    };

    // For convenience
    namespace menu {
        // Open the Horrible Menu mod option menu
        inline void open(bool force = false, std::optional<std::string> theme = std::nullopt) {
            if (auto& old = Menu::get()) {
                cue::resetNode(old);
                if (!force) return;
            };

            if (auto popup = Menu::create(std::move(theme))) popup->show();
        }
    };
};