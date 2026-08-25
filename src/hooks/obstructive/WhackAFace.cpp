#include <Util.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "whack_a_face"

static auto const o = Option::create(THIS_ID)
                          ->setName("Whack-A-Face!")
                          ->setDescription("Buttons with difficulty faces will start to pop up on your screen while playing a level. You'll have a very limited amount of time to press each one until they all disappear, or else you restart the entire level! The higher the difficulty of the face on the button, the more times you'll need to press it.\n<cl>created by Cheeseworks</c>")
                          ->setCategory(category::obstructive)
                          ->setSillyTier(SillyTier::High)
                          ->autoRegister();

class $modify(WhackAFacePlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        uint8_t chance = options::getChance(THIS_ID);

        std::vector<WeakRef<WhackButton>> active;

        void clearWhackBtns() {
            log::trace("Clearing {} whackable button references", active.size());

            for (auto& whackBtn : active) {
                if (auto btn = whackBtn.lock()) btn->removeFromParent();
            };

            active.clear();
            active.shrink_to_fit();
        };
    };

    HORRIBLE_SETUP_INTERFACE_FUNC {
        if (!on) {
            unschedule(schedule_selector(WhackAFacePlayLayer::doWhack));
            m_fields->clearWhackBtns();

            return;
        };

        cursor::show();
        nextWhack();
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        HORRIBLE_SETUP_INTERFACE_FUNC_NAME();
    };

    void levelComplete() {
        PlayLayer::levelComplete();
        m_fields->clearWhackBtns();
    };

    void resetLevel() {
        PlayLayer::resetLevel();

        m_fields->clearWhackBtns();
        cursor::show();
    };

    void resetLevelFromStart() {
        PlayLayer::resetLevelFromStart();

        m_fields->clearWhackBtns();
        cursor::show();
    };

    void onQuit() {
        m_fields->clearWhackBtns();
        PlayLayer::onQuit();
    };

    void destroyPlayer(PlayerObject* player, GameObject* object) {
        PlayLayer::destroyPlayer(player, object);

        if (player->m_isDead) {
            queueInMainThread([self = WeakRef(this)]() {
                if (auto s = self.lock()) {
                    s->unschedule(schedule_selector(WhackAFacePlayLayer::doWhack));
                    s->nextWhack();
                };
            });

            m_fields->clearWhackBtns();
            cursor::show();
        };
    };

    void nextWhack() {
        log::trace("scheduling new whack btn");
        if (!m_hasCompletedLevel) scheduleOnce(schedule_selector(WhackAFacePlayLayer::doWhack), rng::get(12.5f, 1.25f) * rng::chanceToDelayPct(m_fields->chance));
    };

    void doWhack(float) {
        if (!m_isPracticeMode && !m_hasCompletedLevel && !m_playerDied) {
            log::debug("Creating new whackable button");

            if (options::isEnabled(THIS_ID)) {
                if (auto whack = WhackButton::create()) {
                    auto const winSize = m_uiLayer->getScaledContentSize();

                    whack->setCallback([this, whack](bool success) {
                        if (!success) resetLevelFromStart();

                        cursor::show();
                        whack->removeFromParent();
                    });
                    whack->setPosition(CCPoint{winSize.width * rng::get(0.75f, 0.25f), winSize.height * rng::get(0.75f, 0.25f)} / 2.f);

                    m_uiLayer->addChild(whack, HIGHEST_Z);
                    m_fields->active.push_back(whack);
                };

                queueInMainThread([self = WeakRef(this)]() {
                    if (auto s = self.lock()) s->nextWhack();
                });
            };
        };
    };
};

HORRIBLE_TOGGLE_MODIFY(PlayLayer, WhackAFacePlayLayer);