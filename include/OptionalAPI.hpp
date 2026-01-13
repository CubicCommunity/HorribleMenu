#pragma once

#include "Horrible.hpp"

#include <Geode/loader/Event.hpp>
#include <Geode/loader/Dispatch.hpp>

#ifdef MY_MOD_ID
#undef MY_MOD_ID
#endif
#define MY_MOD_ID "arcticwoof.horrible_ideas"

namespace horrible {
    class HorribleOptionEventV2 : public geode::Event {
    private:
        std::string m_id;
        bool m_toggled;

    public:
        HorribleOptionEventV2(std::string id, bool toggled) : m_id(std::move(id)), m_toggled(toggled) {};

        std::string const& getId() const { return m_id; };
        bool getToggled() const { return m_toggled; };
    };

    class HorribleOptionEventFilterV2 : public geode::EventFilter<HorribleOptionEventV2> {
    private:
        std::vector<std::string> m_ids;

    public:
        using Callback = geode::ListenerResult(HorribleOptionEventV2*);

        geode::ListenerResult handle(std::function<Callback> fn, HorribleOptionEventV2* event) {
            if (m_ids.empty()) {
                return fn(event);
            } else {
                for (auto const& id : m_ids) if (event->getId() == id) return fn(event);
            };

            return geode::ListenerResult::Propagate;
        };

        HorribleOptionEventFilterV2() = default;
        HorribleOptionEventFilterV2(std::string id) : m_ids({ std::move(id) }) {};
        HorribleOptionEventFilterV2(std::vector<std::string> ids) : m_ids(std::move(ids)) {};
    };

    class OptionManagerV2 {
    public:
        static geode::Result<> registerOption(Option const& option)
            GEODE_EVENT_EXPORT(&OptionManagerV2::registerOption, (option));

        static geode::Result<bool> getOption(std::string_view id)
            GEODE_EVENT_EXPORT(&OptionManagerV2::getOption, (id));

        static geode::Result<bool> setOption(std::string_view id, bool enable)
            GEODE_EVENT_EXPORT(&OptionManagerV2::setOption, (id, enable));
    };
};