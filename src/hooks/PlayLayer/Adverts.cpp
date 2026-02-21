#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static Option const o = {
    "ads",
    "Level Ads",
    "While playing a level in normal mode, an ad for a random level will pop up on your screen from time to time.\n<cy>Credit: staticGD</c>",
    category::obstructive,
    SillyTier::Medium,
};
HORRIBLE_REGISTER_OPTION(o);

class $modify(AdvertsPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(o.id);

    struct Fields {
        RandomAd* m_ad = nullptr;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        nextAd();
    };

    void nextAd() {
        auto delay = randng::get(15.f, 5.f);
        log::debug("scheduling ad in {}s", delay);

        scheduleOnce(schedule_selector(AdvertsPlayLayer::showAd), delay);
    };

    void showAd(float) {
        auto f = m_fields.self();

        if (f->m_ad) f->m_ad->removeMeAndCleanup();

#ifdef GEODE_IS_WINDOWS
        // Show cursor when ad appears
        CCEGLView::sharedOpenGLView()->showCursor(true);
#endif
        if (auto popup = RandomAd::create()) {
            f->m_ad = popup;
            f->m_ad->show();
        };

        nextAd();
    };
};