#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "ads"

static auto const o = Option::create(THIS_ID)
                          ->setName("Level Ads")
                          ->setDescription("While playing a level in normal mode, an ad for a random level will pop up on your screen from time to time.\n<cl>suggested by staticGD</c>")
                          ->setCategory(category::obstructive)
                          ->setSillyTier(SillyTier::Medium)
                          ->setOnline(true)
                          ->autoRegister();

class $modify(AdvertsPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields {
        Ref<RandomAd> ad = nullptr;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        cursor::show();
        nextAd();
    };

    void nextAd() {
        auto delay = randng::get(15.f, 5.f);
        log::trace("scheduling ad in {}s", delay);

        scheduleOnce(schedule_selector(AdvertsPlayLayer::showAd), delay);
    };

    void showAd(float) {
        auto f = m_fields.self();

        cue::resetNode(f->ad);

        if (auto popup = RandomAd::create()) {
            popup->show();
            f->ad = popup;
        };

        queueInMainThread([self = WeakRef(this)]() {
            if (auto s = self.lock()) s.take()->nextAd();
        });
    };
};