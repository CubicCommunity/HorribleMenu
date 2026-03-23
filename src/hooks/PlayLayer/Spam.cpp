#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static constexpr auto id = "spam";

inline static Option const o = {
    id,
    "Spam Challenge!",
    "Sometimes mercilessly forces a challenge on you to spam an input sometimes while playing a level in Normal mode.\n<cl>Credit: Cheeseworks</c>",
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

    void levelComplete() {
        PlayLayer::levelComplete();
        if (auto spam = WeakRef(m_fields->m_currentSpam).lock()) spam->removeMeAndCleanup();
    };

    void destroyPlayer(PlayerObject* player, GameObject* object) {
        PlayLayer::destroyPlayer(player, object);

        auto f = m_fields.self();

        if (player->m_isDead) {
            if (auto spam = WeakRef(f->m_currentSpam).lock()) {
                log::trace("removing activate spam challenge after player death");

                spam->removeMeAndCleanup();
                nextSpam();
            };

            f->m_currentSpam = nullptr;
        };
    };

    void nextSpam() {
        log::trace("scheduling spam challenge");
        if (!m_hasCompletedLevel) scheduleOnce(schedule_selector(SpamPlayLayer::doSpam), randng::get(30.f, 5.f) * chanceToDelayPct(m_fields->chance));
    };

    void doSpam(float) {
        if (!m_isPracticeMode && !m_hasCompletedLevel && !m_playerDied) {
            log::debug("Showing spam challenge");

            if (options::isEnabled(id)) {
                if (auto spam = SpamChallenge::create()) {
                    auto f = m_fields.self();

                    // handle correct/wrong answer
                    spam->setCallback([self = WeakRef(this), challenge = WeakRef(spam)](bool success) {
                        if (auto s = self.lock()) {
                            log::debug("spam {}", success ? "succeeded" : "failed");

                            if (!success) s->resetLevelFromStart();
                            s->nextSpam();

                            cursor::hide();

                            if (auto spam = challenge.lock()) spam->removeMeAndCleanup();
                        };
                    });

                    m_uiLayer->addChild(spam, 99);
                    f->m_currentSpam = spam;

                    cursor::show();
                };
            } else {
                queueInMainThread([self = WeakRef(this)]() {
                    if (auto s = self.lock()) s->nextSpam();
                });
            };
        };
    };
};