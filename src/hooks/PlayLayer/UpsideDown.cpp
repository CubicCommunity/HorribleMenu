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
HORRIBLE_REGISTER_OPTION(o);

class $modify(ForceLevelsPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(o.id);

    struct Fields {
        int chance = options::getChance(o.id); // chance as percent
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        auto f = m_fields.self();

        int rnd = randng::fast(); // random float between 0 and 100
        log::debug("scene rng {} chance {}", rnd, f->chance);

        if (rnd <= f->chance) {
            log::debug("setting scene upside down");
            setRotation(-180.f);
        };
    };
};