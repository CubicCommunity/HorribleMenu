#include <Utils.h>

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

    struct Fields {
        uint8_t chance = options::getChance(THIS_ID);

        Ref<SpamChallenge> currentSpam = nullptr;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        nextSpam();
    };

    void levelComplete() {
        PlayLayer::levelComplete();
        cue::resetNode(m_fields->currentSpam);
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
        if (!m_hasCompletedLevel) scheduleOnce(schedule_selector(SpamPlayLayer::doSpam), randng::get(30.f, 5.f) * chanceToDelayPct(m_fields->chance));
    };

    void doSpam(float) {
        if (!m_isPracticeMode && !m_hasCompletedLevel && !m_playerDied) {
            log::debug("Showing spam challenge");

            if (options::isEnabled(THIS_ID)) {
                if (auto spam = SpamChallenge::create()) {
                    auto f = m_fields.self();

                    // handle correct/wrong answer
                    spam->setCallback([self = WeakRef(this), challenge = WeakRef(spam)](bool success) {
                        if (auto s = self.lock()) {
                            log::debug("spam {}", success ? "succeeded" : "failed");

                            if (!success) s->resetLevelFromStart();
                            s.take()->nextSpam();

                            cursor::hide();

                            if (auto spam = challenge.lock()) spam.take()->removeFromParent();
                        };
                    });

                    m_uiLayer->addChild(spam, 99);
                    f->currentSpam = spam;

                    cursor::show();
                };
            } else {
                queueInMainThread([self = WeakRef(this)]() {
                    if (auto s = self.lock()) s.take()->nextSpam();
                });
            };
        };
    };
};