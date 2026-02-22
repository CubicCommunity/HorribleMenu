#pragma once

#include "OptionCategoryItem.hpp"

#include <Utils.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible;

class OptionMenu final : public Popup {
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;

    struct SillyFilterBtnData final {
        SillyTier tier;
        const char* label;
        const char* id;
        ccColor3B color;
    };

    using Callback = void(*)(CCMenuItem*);

    struct SocialBtnData final {
        const char* sprite;
        const char* id;
        Callback callback;
    };

protected:
    static OptionMenu* s_inst;

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