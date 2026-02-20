#pragma once

#include "OptionCategoryItem.hpp"

#include <Utils.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible;

class OptionMenuPopup final : public Popup {
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;

    struct SillyFilter final {
        SillyTier tier;
        const char* label;
        ccColor3B color;
    };

protected:
    static OptionMenuPopup* s_inst;

    // EventListener<CategoryEventFilter> m_catListener = {
    //     [this](CategoryEvent* event) {
    //         return OnCategory(event->getId(), event->isEnabled());
    //     },
    //     CategoryEventFilter()
    // };

    OptionMenuPopup();
    ~OptionMenuPopup();

    void filterOptions(std::span<const Option> optList, SillyTier tier = SillyTier::None, ZStringView category = "");
    void filterTierCallback(CCObject*);

    void resetFilters(CCObject*);

    void openModSettings(CCObject*);
    void openSeriesPage(CCObject*);
    void openSupporterPopup(CCObject*);

    void onClose(CCObject* sender) override;
    void onExit() override;
    void cleanup() override;

    bool init() override;

public:
    static OptionMenuPopup* create();

    static OptionMenuPopup* get() noexcept;
};