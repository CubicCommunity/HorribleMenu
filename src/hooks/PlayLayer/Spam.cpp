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
        if (!m_isPracticeMode && !m_hasCompletedLevel) {
            log::debug("Showing spam challenge");

            if (options::isEnabled(id) && !m_playerDied) {
                if (auto spam = SpamChallenge::create()) {
                    auto f = m_fields.self();

                    // handle correct/wrong answer
                    spam->setCallback([this, f](bool success) {
                        log::debug("spam {}", success ? "succeeded" : "failed");
                        if (!success) resetLevelFromStart();
                        nextSpam();

                        queueInMainThread([f]() {
                            if (f) f->m_currentSpam = nullptr;
                            });
                        });

#ifdef GEODE_IS_WINDOWS
                    CCEGLView::sharedOpenGLView()->showCursor(true);
#endif
                    m_uiLayer->addChild(spam, 99);
                    f->m_currentSpam = spam;
                };
            } else {
                queueInMainThread([this]() {
                    nextSpam();
                    });
            };
        };
    };

    void destroyPlayer(PlayerObject * player, GameObject * object) {
        PlayLayer::destroyPlayer(player, object);

        auto f = m_fields.self();

        if (player->m_isDead) {
            if (f->m_currentSpam) {
                log::trace("removing activate spam challenge after player death");
                f->m_currentSpam->removeMeAndCleanup();
                nextSpam();
            };
        };
    };
};