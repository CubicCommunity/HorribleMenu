#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static constexpr auto id = "whack_a_face";

inline static Option const o = {
    id,
    "Whack-A-Face!",
    "Buttons with difficulty faces will start to appear on your screen while playing a level. You have a very limited amount of time to press each one until they all disappear, or you restart the whole level from 0%! The higher the difficulty of the face on the button, the more time you'll be required to press it.\n<cl>Credit: Cheeseworks</c>",
    category::obstructive,
    SillyTier::High,
};
HORRIBLE_REGISTER_OPTION(o);

class $modify(WhackAFacePlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(id);

    struct Fields {
        int chance = options::getChance(id);

        std::vector<WeakRef<WhackButton>> active;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        nextWhack();
        cursor::show();
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
        };
    };

    void nextWhack() {
        log::trace("scheduling new whack btn");
        if (!m_hasCompletedLevel) scheduleOnce(schedule_selector(WhackAFacePlayLayer::doWhack), randng::get(15.f, 2.5f) * chanceToDelayPct(m_fields->chance));
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