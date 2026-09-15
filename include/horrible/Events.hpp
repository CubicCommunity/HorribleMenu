#pragma once

#include "Option.hpp"

#include <Geode/loader/Event.hpp>

#include <Geode/utils/cocos.hpp>
#include <Geode/utils/function.hpp>

namespace horrible {
    // Event for option toggles
    class OptionEvent final : public geode::ThreadSafeGlobalEvent<OptionEvent, bool(std::string_view, OptionSave), bool(OptionSave), std::string> {
    public:
        using ThreadSafeGlobalEvent::ThreadSafeGlobalEvent;
    };

    struct OptionCheatingEvent final : public geode::Event<OptionEvent, bool(bool)> {
        using Event::Event;
    };

    struct MenuEvent final : geode::Event<MenuEvent, bool(geode::Ref<geode::Popup>, bool)> {
        using Event::Event;
    };

    inline geode::ListenerHandle* listenForHorribleOptionChanges(std::string id, geode::CopyableFunction<void(OptionSave)>&& callback) {
        return OptionEvent(std::move(id)).listen(std::move(callback)).leak();
    };

    inline geode::ListenerHandle* listenForAllHorribleOptionChanges(geode::CopyableFunction<void(std::string_view, OptionSave)>&& callback) {
        return OptionEvent().listen(std::move(callback)).leak();
    };
};