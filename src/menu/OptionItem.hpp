#pragma once

#include <Utils.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible;

class OptionItem : public CCMenu {
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;

protected:
    OptionItem();
    virtual ~OptionItem();

    void saveTogglerState();

    void onToggle(CCObject*);
    void onDescription(CCObject*);
    void onExit() override;

    bool init(CCSize const& size, Option const& option);

public:
    static OptionItem* create(CCSize const& size, Option const& option);

    Option getOption() const;
    bool isCompatible() const;
};