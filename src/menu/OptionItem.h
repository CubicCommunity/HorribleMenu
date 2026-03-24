#pragma once

#include <Utils.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible;

namespace horrible {
    // Event for pin toggles
    struct PinEvent final : Event<PinEvent, bool()> {
        using Event::Event;
    };

    class OptionItem final : public CCMenu, private FLAlertLayerProtocol {
    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;

    protected:
        OptionItem();
        ~OptionItem();

        void onToggle(CCObject*);
        void onPin(CCObject* sender);

        bool init(CCSize const& size, Option option, bool devMode);

    public:
        static OptionItem* create(CCSize const& size, Option option, bool devMode);

        Option const& getOption() const noexcept;
        bool isCompatible() const noexcept;
    };
};