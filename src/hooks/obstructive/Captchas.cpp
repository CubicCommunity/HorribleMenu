#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "captcha"

static auto const o = Option::create(THIS_ID)
                          ->setName("Verify Your Captchas...")
                          ->setDescription("Occasionally, you will be prompted to verify you're not a bot with a good ole' captcha.\n<cl>suggested by bonieGPT</c>")
                          ->setCategory(category::obstructive)
                          ->setSillyTier(SillyTier::High)
                          ->setCheating(true)
                          ->autoRegister();

class $modify(CaptchaPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        uint8_t chance = options::getChance(THIS_ID);

        Ref<Captcha> currentCaptcha = nullptr;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        nextCaptcha();
    };

    void resetLevelFromStart() {
        PlayLayer::resetLevelFromStart();
        cue::resetNode(m_fields->currentCaptcha);
    };

    void levelComplete() {
        PlayLayer::levelComplete();
        cue::resetNode(m_fields->currentCaptcha);
    };

    void destroyPlayer(PlayerObject* player, GameObject* object) {
        PlayLayer::destroyPlayer(player, object);

        if (player->m_isDead) {
            cue::resetNode(m_fields->currentCaptcha);
            nextCaptcha();
        };
    };

    void nextCaptcha() {
        log::trace("scheduling captcha");

        unschedule(schedule_selector(CaptchaPlayLayer::doCaptcha));
        if (!m_hasCompletedLevel) scheduleOnce(schedule_selector(CaptchaPlayLayer::doCaptcha), rng::get(30.f, 5.f) * chanceToDelayPct(m_fields->chance));
    };

    void doCaptcha(float) {
        auto f = m_fields.self();

        if (options::isEnabled(THIS_ID) && !f->currentCaptcha && !m_hasCompletedLevel && !m_playerDied) {
            if (auto captcha = Captcha::create()) {
                captcha->setCallback([this](bool success) {
                    cursor::hide();
                    m_player1->m_playerSpeed = 1.f;
                    m_player2->m_playerSpeed = 1.f;

                    if (!success) {
                        Notification::create("Knew you were a robot...", NotificationIcon::Error)->show();
                        resetLevelFromStart();
                    };

                    cue::resetNode(m_fields->currentCaptcha);
                });

                captcha->show();
                f->currentCaptcha = captcha;

                cursor::show();

                m_player1->m_playerSpeed = 0.125f;
                m_player2->m_playerSpeed = 0.125f;
            };
        };

        queueInMainThread([self = WeakRef(this)]() {
            if (auto s = self.lock()) s->nextCaptcha();
        });
    };
};