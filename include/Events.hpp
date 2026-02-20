#pragma once

#include "Horrible.hpp"

#include <Geode/loader/Event.hpp>

// Container for Horrible Ideas API
namespace horrible {
    // Event for option toggles
    struct OptionEvent final : geode::GlobalEvent<OptionEvent, bool(std::string, bool), std::string> {
        using GlobalEvent::GlobalEvent;
    };
};