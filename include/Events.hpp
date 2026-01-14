#pragma once

#include "Horrible.hpp"

#include <cocos2d.h>

#include <Geode/loader/Event.hpp>

// Container for Horrible Ideas API
namespace horrible {
    // Event for option toggles
    class HorribleOptionEvent : public geode::Event {
    private:
        std::string m_id; // Unique ID of the option
        bool m_toggled; // Toggle boolean of the option

    public:
        HorribleOptionEvent(std::string id, bool toggled); // Constructor

        AWCW_HORRIBLE_API_DLL std::string const& getId() const; // Get the unique ID of the option
        AWCW_HORRIBLE_API_DLL bool getToggled() const; // Get the toggle boolean of the option
    };

    // Filter for option toggle event
    class AWCW_HORRIBLE_API_DLL HorribleOptionEventFilter : public geode::EventFilter<HorribleOptionEvent> {
    private:
        std::vector<std::string> m_ids; // Unique ID of the options to listen to

    public:
        using Callback = geode::ListenerResult(HorribleOptionEvent*);

        /**
         * Event handler
         *
         * @param fn Callback function containing a pointer to the event that fired
         * @param event Pointer to the event that fired
         */
        geode::ListenerResult handle(std::function<Callback> fn, HorribleOptionEvent* event);

        HorribleOptionEventFilter() = default; // Constructor

        HorribleOptionEventFilter(std::string id); // Constructor (listens to one option's toggle)
        HorribleOptionEventFilter(std::vector<std::string> ids); // Constructor (listens to any specified options' toggles)

        HorribleOptionEventFilter(cocos2d::CCNode*, std::string id); // Constructor with target (listens to one option's toggle)
        HorribleOptionEventFilter(cocos2d::CCNode*, std::vector<std::string> ids); // Constructor with target (listens to any specified options' toggles)
    };
};