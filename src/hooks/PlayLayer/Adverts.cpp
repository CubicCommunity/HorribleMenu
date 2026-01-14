#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class $modify(AdvertsPlayLayer, PlayLayer) {
    struct Fields {
        bool enabled = options::get("ads");

        RandomAd* m_ad = nullptr;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        auto f = m_fields.self();

        this->template addEventListener<OptionEventFilter>(
            [this, f](OptionEvent* ev) {
                unschedule(schedule_selector(AdvertsPlayLayer::showAd));

                f->enabled = ev->getToggled();

                if (f->enabled) nextAd();

                return ListenerResult::Propagate;
            },
            "ads"
        );

        if (f->enabled) nextAd();
    };

    void nextAd() {
        auto delay = randng::get(15.f, 5.f);
        log::debug("scheduling ad in {}s", delay);

        if (m_fields->enabled) scheduleOnce(schedule_selector(AdvertsPlayLayer::showAd), delay);
    };

    void showAd(float) {
        auto f = m_fields.self();

        if (f->enabled) {
            if (f->m_ad) f->m_ad->removeMeAndCleanup();

#ifdef GEODE_IS_WINDOWS
            // Show cursor when ad appears
            CCEGLView::sharedOpenGLView()->showCursor(true);
#endif
            if (auto popup = RandomAd::create()) {
                f->m_ad = popup;
                f->m_ad->show();
            };
        };

        nextAd();
    };
};