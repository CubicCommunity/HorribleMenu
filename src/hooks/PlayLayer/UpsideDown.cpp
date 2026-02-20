#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static Option const o = {
    "upside_down",
    "Upside-Down Chance",
    "While navigating the game, there's a chance a layer will be upside-down, and probably break everything...\n<cy>Credit: Cheeseworks</c>",
    category::chances,
    SillyTier::Medium,
};
REGISTER_HORRIBLE_OPTION(o);

class $modify(ForceLevelsPlayLayer, PlayLayer) {
    struct Fields {
        bool enabled = options::get(o.id);
        int chance = options::getChance(o.id); // chance as percent
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        auto f = m_fields.self();

        if (f->enabled) {
            int rnd = randng::fast(); // random float between 0 and 100
            log::debug("scene rng {} chance {}", rnd, f->chance);

            if (rnd <= f->chance) {
                log::debug("setting scene upside down");
                setRotation(-180.f);
            };
        };
    };
};