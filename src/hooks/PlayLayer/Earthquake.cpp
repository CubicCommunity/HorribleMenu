#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static constexpr auto id = "earthquake";

inline static Option const o = {
    id,
    "Earthquake",
    "Constantly shakes the camera while playing a level.\n<cy>Credit: ArcticWoof</c>",
    category::obstructive,
    SillyTier::Medium,
};
HORRIBLE_REGISTER_OPTION(o);

class $modify(EarthquakePlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(id);

    void setupHasCompleted() {
        scheduleOnce(schedule_selector(EarthquakePlayLayer::nextQuake), 0.125f);
        PlayLayer::setupHasCompleted();
    };

    void nextQuake(float) {
        auto delay = randng::get(3.f, 1.f);
        log::debug("scheduling quake in {}s", delay);

        scheduleOnce(schedule_selector(EarthquakePlayLayer::quake), delay);
    };

    void quake(float) {
        // shake the camera randomly based on intensity
        int rnd = randng::fast();

        shakeCamera(randng::get(5.f, 1.f), randng::get(10.f, 1.f), 0.00875F);

        scheduleOnce(schedule_selector(EarthquakePlayLayer::nextQuake), 0.125f);
    };
};