#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class $modify(ForceLevelsPlayLayer, PlayLayer) {
    struct Fields {
        bool enabled = options::get(key::upside_down);
        int chance = options::getChance(key::upside_down); // chance as percent
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