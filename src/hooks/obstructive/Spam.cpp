#include <Util.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "spam"

static auto const o = Option::create(THIS_ID)
                          ->setName("Spam Challenge!")
                          ->setDescription("Sometimes mercilessly forces a challenge on you to spam an input sometimes while playing a level in Normal mode.\n<cl>created by Cheeseworks</c>")
                          ->setCategory(category::obstructive)
                          ->setSillyTier(SillyTier::High)
                          ->autoRegister();

class $modify(SpamPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        uint8_t chance = options::getChance(THIS_ID);

        Ref<SpamChallenge> currentSpam = nullptr;
    };

    HORRIBLE_SETUP_INTERFACE_FUNC {
        if (!on) {
            cue::resetNode(m_fields->currentSpam);

            unschedule(schedule_selector(SpamPlayLayer::doSpam));

            return;
        };

        nextSpam();
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        HORRIBLE_SETUP_INTERFACE_FUNC_NAME();
    };

    void resetLevelFromStart() {
        PlayLayer::resetLevelFromStart();
        cue::resetNode(m_fields->currentSpam);
    };

    void levelComplete() {
        PlayLayer::levelComplete();
        cue::resetNode(m_fields->currentSpam);
    };

    void togglePracticeMode(bool practiceMode) {
        PlayLayer::togglePracticeMode(practiceMode);

        cue::resetNode(m_fields->currentSpam);
        nextSpam();
    };

    void destroyPlayer(PlayerObject* player, GameObject* object) {
        PlayLayer::destroyPlayer(player, object);

        if (player->m_isDead) {
            cue::resetNode(m_fields->currentSpam);
            nextSpam();
        };
    };

    void nextSpam() {
        log::trace("scheduling spam challenge");

        unschedule(schedule_selector(SpamPlayLayer::doSpam));
        if (!m_hasCompletedLevel) scheduleOnce(schedule_selector(SpamPlayLayer::doSpam), rng::get(30.f, 5.f) * chanceToDelayPct(m_fields->chance));
    };

    void doSpam(float) {
        auto f = m_fields.self();

        if (options::isEnabled(THIS_ID) && !f->currentSpam && !m_isPracticeMode && !m_hasCompletedLevel && !m_playerDied) {
            log::debug("Showing spam challenge");

            if (auto spam = SpamChallenge::create()) {
                auto f = m_fields.self();

                // handle correct/wrong answer
                spam->setCallback([this](bool success) {
                    if (!success) resetLevelFromStart();
                    nextSpam();

                    cursor::hide();

                    cue::resetNode(m_fields->currentSpam);
                });

                m_uiLayer->addChild(spam, HIGHEST_Z);
                f->currentSpam = spam;

                cursor::show();
            };
        };

        queueInMainThread([self = WeakRef(this)]() {
            if (auto s = self.lock()) s->nextSpam();
        });
    };
};

HORRIBLE_TOGGLE_MODIFY(PlayLayer, SpamPlayLayer);