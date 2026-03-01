#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static constexpr auto id = "spam";

inline static Option const o = {
    id,
    "Spam Challenge!",
    "Sometimes mercilessly forces a challenge on you to spam an input sometimes while playing a level in Normal mode.\n<cy>Credit: Cheeseworks</c>",
    category::obstructive,
    SillyTier::High,
};
HORRIBLE_REGISTER_OPTION(o);

class $modify(SpamPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(id);

    struct Fields {
        int chance = options::getChance(id);

        SpamChallenge* m_currentSpam = nullptr;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        nextSpam();
    };

    void nextSpam() {
        log::debug("scheduling spam challenge");
        if (!m_hasCompletedLevel) scheduleOnce(schedule_selector(SpamPlayLayer::doSpam), randng::get(30.f, 5.f) * chanceToDelayPct(m_fields->chance));
    };

    void doSpam(float) {
        auto f = m_fields.self();

        if (!m_isPracticeMode && !m_hasCompletedLevel) {
            log::info("Showing spam challenge");

            if (auto spam = SpamChallenge::create()) {
                // handle correct/wrong answer
                spam->setCallback([this](bool success) {
                    nextSpam();
                    if (!success) resetLevelFromStart();
                    });

#ifdef GEODE_IS_WINDOWS
                CCEGLView::sharedOpenGLView()->showCursor(true);
#endif
                m_uiLayer->addChild(spam, 99);
                f->m_currentSpam = spam;
            };
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
        };
    };
};