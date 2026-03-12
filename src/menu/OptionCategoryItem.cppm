module;

#include <Geode/Geode.hpp>

using namespace geode::prelude;

export module OptionCategoryItem;

// Event for option toggles
export struct CategoryEvent final : Event<CategoryEvent, bool(std::string_view, bool)> {
    using Event::Event;
};

export class OptionCategoryItem final : public CCMenu {
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;

protected:
    OptionCategoryItem();
    ~OptionCategoryItem();

    void onToggle(CCObject* sender);

    bool init(CCSize const& size, std::string category);

public:
    static OptionCategoryItem* create(CCSize const& size, std::string category);
};