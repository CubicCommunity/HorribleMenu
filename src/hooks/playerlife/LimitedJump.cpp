#include <Util.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "limited_jump"

static auto const o = Option::create(THIS_ID)
                          ->setName("Limited Jump Energy")
                          ->setDescription("You only have a limited amount of jump inputs you can make for a period of time, which will regenerate at a random pace over time. Run out of available inputs too quickly, and your character will die.\n<cl>created by Cheeseworks</c>")
                          ->setCategory(category::playerlife)
                          ->setSillyTier(SillyTier::Medium)
                          ->autoRegister();

class $modify(LimitedJumpGJBaseGameLayer, GJBaseGameLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        JumpHealthMeter* jumpMeter = nullptr;
    };

    HORRIBLE_SETUP_INTERFACE_FUNC {
        auto f = m_fields.self();

        if (!on) return cue::resetNode(f->jumpMeter);

        if (auto pl = PlayLayer::get()) {
            f->jumpMeter = JumpHealthMeter::create([this, meter = f->jumpMeter]() {
                if (auto pl = PlayLayer::get()) pl->destroyPlayer(m_player1, nullptr);
            });
            f->jumpMeter->setID("jump-energy-meter"_spr);
            f->jumpMeter->setZOrder(HIGHEST_Z);
            f->jumpMeter->setAnchorPoint({1, 0});

            pl->m_uiLayer->addChildAtPosition(f->jumpMeter, Anchor::BottomRight, {-8.75f, 8.75f}, false);
        };
    };

    bool init() {
        if (!GJBaseGameLayer::init()) return false;

        queueInMainThread([self = WeakRef(this)]() {
            if (auto s = self.lock()) s->HORRIBLE_SETUP_INTERFACE_FUNC_NAME();
        });

        return true;
    };

    void handleButton(bool down, int button, bool isPlayer1) {
        GJBaseGameLayer::handleButton(down, button, isPlayer1);
        if (button == 1 && down) m_fields->jumpMeter->jump();
    };

    JumpHealthMeter* getHealthMeter() noexcept {
        if (auto f = m_fields.self()) return f->jumpMeter;
        return nullptr;
    };
};

class $modify(LimitedJumpPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    void destroyPlayer(PlayerObject* player, GameObject* object) {
        PlayLayer::destroyPlayer(player, object);

        if (m_playerDied) {
            if (auto gjbgl = GJBaseGameLayer::get()) {
                if (auto meter = modify_cast<LimitedJumpGJBaseGameLayer*>(gjbgl)->getHealthMeter()) meter->playPause(false);
            };
        };
    };

    void resetLevel() {
        PlayLayer::resetLevel();

        if (auto gjbgl = GJBaseGameLayer::get()) {
            if (auto meter = modify_cast<LimitedJumpGJBaseGameLayer*>(gjbgl)->getHealthMeter()) meter->reset();
        };
    };

    void resetLevelFromStart() {
        PlayLayer::resetLevelFromStart();

        if (auto gjbgl = GJBaseGameLayer::get()) {
            if (auto meter = modify_cast<LimitedJumpGJBaseGameLayer*>(gjbgl)->getHealthMeter()) meter->reset();
        };
    };
};

HORRIBLE_TOGGLE_MODIFY(GJBaseGameLayer, LimitedJumpGJBaseGameLayer);