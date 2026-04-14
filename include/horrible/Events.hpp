#pragma once

#include "Option.hpp"

#include <Geode/loader/Event.hpp>

#include <Geode/utils/function.hpp>

namespace horrible {
    // Event for option toggles
    class OptionEvent final : public geode::ThreadSafeGlobalEvent<OptionEvent, bool(std::string_view, HorribleOptionSave), bool(HorribleOptionSave), std::string> {
    public:
        using ThreadSafeGlobalEvent::ThreadSafeGlobalEvent;
    };

    inline geode::ListenerHandle* listenForHorribleOptionChanges(std::string id, geode::CopyableFunction<void(HorribleOptionSave)> callback) {
        return OptionEvent(std::move(id)).listen(std::move(callback)).leak();
    };

    inline geode::ListenerHandle* listenForAllHorribleOptionChanges(geode::CopyableFunction<void(std::string, HorribleOptionSave)> callback) {
        return OptionEvent().listen(std::move(callback)).leak();
    };
};