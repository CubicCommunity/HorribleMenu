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

    inline geode::ListenerHandle* listenForHorribleOptionChanges(std::string id, geode::FunctionRef<bool(HorribleOptionSave)> callback) {
        return OptionEvent(std::move(id)).listen([callback](HorribleOptionSave data) {
                                             return callback(data);
                                         })
            .leak();
    };

    inline geode::ListenerHandle* listenForAllHorribleOptionChanges(geode::FunctionRef<bool(std::string_view, HorribleOptionSave)> callback) {
        return OptionEvent().listen([callback](std::string_view id, HorribleOptionSave data) {
                                return callback(id, data);
                            })
            .leak();
    };
};