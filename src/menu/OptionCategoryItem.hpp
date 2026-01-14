#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

// Event for option toggles
class CategoryEvent : public Event {
protected:
    std::string m_id;
    bool m_enabled;

public:
    CategoryEvent(std::string id, bool enabled = false);

    std::string_view getId() const noexcept;
    bool isEnabled() const noexcept;
};

// Filter for option toggle event
class CategoryEventFilter : public EventFilter<CategoryEvent> {
public:
    using Callback = ListenerResult(CategoryEvent*);
    ListenerResult handle(std::function<Callback> fn, CategoryEvent* event);
};

class OptionCategoryItem : public CCMenu {
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;

protected:
    EventListener<CategoryEventFilter> m_listener = {
        [this](CategoryEvent* event) {
            return OnCategory(event->getId(), event->isEnabled());
        },
        CategoryEventFilter()
    };

    OptionCategoryItem();
    virtual ~OptionCategoryItem();

    ListenerResult OnCategory(std::string_view category, bool enabled = false);

    void onToggle(CCObject* sender);

    bool init(CCSize const& size, std::string_view category);

public:
    static OptionCategoryItem* create(CCSize const& size, std::string_view category);
};