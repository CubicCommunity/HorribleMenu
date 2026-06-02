#include <Utils.h>

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

    void setupHasCompleted() {
        scheduleOnce(schedule_selector(EarthquakePlayLayer::nextQuake), 0.125f);
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