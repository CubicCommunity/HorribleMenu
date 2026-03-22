#pragma once

#include "OptionCategoryItem.hpp"

#include <Utils.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace horrible {
    class OptionMenu final : public Popup {
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
        };

        void setupSafeModeNode(bool safeMode);

    protected:
        OptionMenu();
        ~OptionMenu();

        void onClose(CCObject* sender) override;
        void onExit() override;
        void cleanup() override;

        bool init() override;

    public:
        static OptionMenu* create();

        static OptionMenu* get() noexcept;
    };
};