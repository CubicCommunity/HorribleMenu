#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

// Event for option toggles
struct CategoryEvent final : Event<CategoryEvent, bool(std::string, bool)> {
    using Event::Event;
};

class OptionCategoryItem final : public CCMenu {
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;

protected:
    OptionCategoryItem();
    ~OptionCategoryItem();

    ListenerResult OnCategory(std::string_view category, bool enabled = false);

    void onToggle(CCObject* sender);

    bool init(CCSize const& size, std::string category);

public:
    static OptionCategoryItem* create(CCSize const& size, std::string category);
};