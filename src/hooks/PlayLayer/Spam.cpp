#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static Option const o = {
        "spam",
        "Spam Challenge!",
        "Sometimes forces a challenge on you to mercilessly spam an input sometimes while playing a level in Normal mode.\n<cy>Credit: Cheeseworks</c>",
        category::obstructive,
        SillyTier::High,
};
REGISTER_HORRIBLE_OPTION(o);

class $modify(SpamPlayLayer, PlayLayer) {
    struct Fields {
        bool enabled = options::get(o.id);
        int chance = options::getChance(o.id);

        SpamChallenge* m_currentSpam = nullptr;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        auto f = m_fields.self();

        if (f->enabled) nextSpam();
    };

    void nextSpam() {
        log::debug("scheduling spam challenge");
        if (m_fields->enabled && !m_hasCompletedLevel) scheduleOnce(schedule_selector(SpamPlayLayer::doSpam), randng::get(30.f, 5.f) * chanceToDelayPct(m_fields->chance));
    };

    void doSpam(float) {
        auto f = m_fields.self();

        if (f->enabled && !m_isPracticeMode && !m_hasCompletedLevel) {
            log::info("Showing spam challenge");

            if (auto spam = SpamChallenge::create()) {
                f->m_currentSpam = spam;

                // clear pointer on close / handle correct/wrong answer
                f->m_currentSpam->setCallback([this, f](bool success) {
                    nextSpam();
                    if (!success) resetLevelFromStart();

                    if (f->m_currentSpam) f->m_currentSpam->removeMeAndCleanup();
                    f->m_currentSpam = nullptr;
                                              });

#ifdef GEODE_IS_WINDOWS
                CCEGLView::sharedOpenGLView()->showCursor(true);
#endif
                m_uiLayer->addChild(f->m_currentSpam, 99);
            };
        } else if (f->enabled) {
            nextSpam();
        };
    };

    void destroyPlayer(PlayerObject * player, GameObject * object) {
        PlayLayer::destroyPlayer(player, object);

        auto f = m_fields.self();

        if (m_player1->m_isDead) {
            if (f->m_currentSpam) {
                f->m_currentSpam->removeMeAndCleanup();
                nextSpam();
            };

            f->m_currentSpam = nullptr;
        };
    };
};