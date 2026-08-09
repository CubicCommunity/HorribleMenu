#include <Util.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "earthquake"

static auto const o = Option::create(THIS_ID)
                          ->setName("Earthquake")
                          ->setDescription("Constantly shakes the camera while playing a level.\n<cl>created by ArcticWoof</c>")
                          ->setCategory(category::obstructive)
                          ->setSillyTier(SillyTier::Low)
                          ->autoRegister();

class $modify(EarthquakePlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    HORRIBLE_SETUP_INTERFACE_FUNC {
        if (!on) {
            unschedule(schedule_selector(EarthquakePlayLayer::nextQuake));
            unschedule(schedule_selector(EarthquakePlayLayer::quake));

            return;
        };

        scheduleOnce(schedule_selector(EarthquakePlayLayer::nextQuake), 0.125f);
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
    };

    void nextQuake(float) {
        auto delay = rng::get(3.75f, 1.25f);
        log::trace("scheduling quake in {}s", delay);

        scheduleOnce(schedule_selector(EarthquakePlayLayer::quake), delay);
    };

    void quake(float) {
        shakeCamera(rng::get(2.5f, 1.f), rng::get(5.f, 1.f), 0.00125f);
        scheduleOnce(schedule_selector(EarthquakePlayLayer::nextQuake), 0.125f);
    };
};

HORRIBLE_TOGGLE_MODIFY(PlayLayer, EarthquakePlayLayer);