#pragma once

#include "Horrible.hpp"

#include <Geode/loader/Event.hpp>
#include <Geode/loader/Dispatch.hpp>

#ifdef MY_MOD_ID
#undef MY_MOD_ID
#endif
#define MY_MOD_ID "arcticwoof.horrible_ideas"

namespace horrible {
    class OptionEventV2 : public geode::Event {
    private:
        std::string m_id;
        bool m_toggled;

    public:
        OptionEventV2(std::string id, bool toggled) : m_id(std::move(id)), m_toggled(toggled) {};

        std::string_view getId() const noexcept { return m_id; };
        bool getToggled() const noexcept { return m_toggled; };
    };

    class OptionEventFilterV2 : public geode::EventFilter<OptionEventV2> {
    private:
        std::vector<std::string> m_ids;

    public:
        using Callback = geode::ListenerResult(OptionEventV2*);

        geode::ListenerResult handle(std::function<Callback> fn, OptionEventV2* event) {
            if (m_ids.empty()) {
                return fn(event);
            } else {
                for (auto const& id : m_ids) if (event->getId() == id) return fn(event);
            };

            return geode::ListenerResult::Propagate;
        };

        OptionEventFilterV2() = default;
        OptionEventFilterV2(std::string id) : m_ids({ std::move(id) }) {};
        OptionEventFilterV2(std::vector<std::string> ids) : m_ids(std::move(ids)) {};
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