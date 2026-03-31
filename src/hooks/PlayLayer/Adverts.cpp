#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

static constexpr auto id = "ads";

static auto const o = Option::create(id)
                          .setName("Level Ads")
                          .setDescription("While playing a level in normal mode, an ad for a random level will pop up on your screen from time to time.\n<co>An internet connection is required.</c>\n<cl>suggested by staticGD</c>")
                          .setCategory(category::obstructive)
                          .setSillyTier(SillyTier::Medium);
HORRIBLE_REGISTER_OPTION(o);

class $modify(AdvertsPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(id);

    struct Fields {
        RandomAd* ad = nullptr;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        nextAd();
    };

    void nextAd() {
        auto delay = randng::get(15.f, 5.f);
        log::trace("scheduling ad in {}s", delay);

        scheduleOnce(schedule_selector(AdvertsPlayLayer::showAd), delay);
    };

    void showAd(float) {
        auto f = m_fields.self();

        if (f->ad) f->ad->removeMeAndCleanup();

        if (auto popup = RandomAd::create()) {
            f->ad = popup;
            f->ad->show();
        };

        nextAd();
    };
};