#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

static constexpr auto id = "whack_a_face";

static auto const o = Option::create(id)
                          .setName("Whack-A-Face!")
                          .setDescription("Buttons with difficulty faces will start to pop up on your screen while playing a level. You'll have a very limited amount of time to press each one until they all disappear, or else you restart the entire level! The higher the difficulty of the face on the button, the more times you'll need to press it.\n<cl>created by Cheeseworks</c>")
                          .setCategory(category::obstructive)
                          .setSillyTier(SillyTier::High);
HORRIBLE_REGISTER_OPTION(o);

class $modify(WhackAFacePlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(id);

    struct Fields {
        unsigned int chance = options::getChance(id);

        std::vector<WeakRef<WhackButton>> active;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        cursor::show();
        nextWhack();
    };

    void levelComplete() {
        PlayLayer::levelComplete();
        clearWhackBtns();
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

            clearWhackBtns();
            cursor::show();
        };
    };

    void resetLevel() {
        PlayLayer::resetLevel();

        clearWhackBtns();
        cursor::show();
    };

    void resetLevelFromStart() {
        PlayLayer::resetLevelFromStart();

        clearWhackBtns();
        cursor::show();
    };

    void nextWhack() {
        log::trace("scheduling new whack btn");
        if (!m_hasCompletedLevel) scheduleOnce(schedule_selector(WhackAFacePlayLayer::doWhack), randng::get(12.5f, 1.25f) * chanceToDelayPct(m_fields->chance));
    };

    void doWhack(float) {
        if (!m_isPracticeMode && !m_hasCompletedLevel && !m_playerDied) {
            log::debug("Creating new whackable button");

            if (options::isEnabled(id)) {
                if (auto whack = WhackButton::create()) {
                    auto const winSize = m_uiLayer->getScaledContentSize();

                    whack->setCallback([self = WeakRef(this), whackButton = WeakRef(whack)](bool success) {
                        if (auto whack = whackButton.lock()) {
                            if (!success) {
                                if (auto s = self.lock()) s->resetLevelFromStart();
                            };

                            cursor::show();
                            whack->removeMeAndCleanup();
                        };
                    });
                    whack->setPosition(CCPoint{winSize.width * randng::get(0.75f, 0.25f), winSize.height * randng::get(0.75f, 0.25f)} / 2.f);

                    m_uiLayer->addChild(whack, 9);
                    m_fields->active.push_back(whack);
                };

                queueInMainThread([self = WeakRef(this)]() {
                    if (auto s = self.lock()) s->nextWhack();
                });
            };
        };
    };

    void clearWhackBtns() {
        auto f = m_fields.self();

        for (auto& whackBtn : f->active) {
            if (auto whack = whackBtn.lock()) whack->removeMeAndCleanup();
        };

        f->active.clear();
        f->active.shrink_to_fit();
    };
};