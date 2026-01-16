#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class $modify(EarthquakePlayLayer, PlayLayer) {
    struct Fields {
        bool enabled = options::get(key::earthquake);
    };

    void setupHasCompleted() {
        if (m_fields->enabled) scheduleOnce(schedule_selector(EarthquakePlayLayer::nextQuake), 0.125f);
        PlayLayer::setupHasCompleted();
    };

    void nextQuake(float) {
        auto delay = randng::get(3.f, 1.f);
        log::debug("scheduling quake in {}s", delay);

        if (m_fields->enabled) scheduleOnce(schedule_selector(EarthquakePlayLayer::quake), delay);
    };

    void quake(float) {
        if (m_fields->enabled) {
            // shake the camera randomly based on intensity
            int rnd = randng::fast();

            shakeCamera(randng::get(5.f, 1.f), randng::get(10.f, 1.f), 0.00875F);
        };

        scheduleOnce(schedule_selector(EarthquakePlayLayer::nextQuake), 0.125f);
    };
};