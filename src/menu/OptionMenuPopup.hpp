#pragma once

#include "OptionCategoryItem.hpp"

#include <Utils.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible;

struct FilterBtnInfo {
    SillyTier tier;
    const char* label;
    ccColor3B color;
};

class OptionMenuPopup : public Popup<> {
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;

protected:
    static OptionMenuPopup* s_inst;

    EventListener<OptionEventFilter> m_listener = {
        [](OptionEvent* event) {
            log::debug("{} option of ID '{}'", event->getToggled() ? "Enabled" : "Disabled", event->getId());
            return ListenerResult::Propagate;
        },
        OptionEventFilter()
    };

    EventListener<CategoryEventFilter> m_catListener = {
        [this](CategoryEvent* event) {
            return OnCategory(event->getId(), event->isEnabled());
        },
        CategoryEventFilter()
    };

    OptionMenuPopup();
    virtual ~OptionMenuPopup();

    ListenerResult OnCategory(std::string_view category, bool enabled = true);

    void filterOptions(std::vector<Option> const& optList, SillyTier tier = SillyTier::None, std::string_view category = "");
    void filterTierCallback(CCObject*);

    void resetFilters(CCObject*);

    void openModSettings(CCObject*);
    void openSeriesPage(CCObject*);
    void openSupporterPopup(CCObject*);

    void onClose(CCObject* sender) override;
    void onExit() override;
    void cleanup() override;

    bool setup() override;

public:
    static OptionMenuPopup* create();

    static OptionMenuPopup* get();
};