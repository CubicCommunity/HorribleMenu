#include <Util.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "input_roulette"

static auto const o = Option::create(THIS_ID)
                          ->setName("Input Roulette")
                          ->setDescription("A meter will show on the right-hand side of the screen during gameplay indicating the safe and not-so-safe times to press your jump button. If the arrow happens to be in an unsafe area when you press jump, you're at risk. Press while <cy>yellow</c>, you have a 50/50 chance of dying, press while <cr>red</c>, and you're guaranteed to die. Press while <cg>green</c>, and you're safe.\n<cl>created by Cheeseworks</c>")
                          ->setCategory(category::misc)
                          ->setSillyTier(SillyTier::High)
                          ->autoRegister();

class $modify(InputRouletteGJBaseGameLayer, GJBaseGameLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        InputMeter* inputMeter = nullptr;
    };

    HORRIBLE_SETUP_INTERFACE_FUNC {
        auto f = m_fields.self();

        if (!on) return cue::resetNode(f->inputMeter);

        if (auto pl = PlayLayer::get()) {
            f->inputMeter = InputMeter::create();
            f->inputMeter->setID("input-chance-meter"_spr);
            f->inputMeter->setScale(1.25f);
            f->inputMeter->setZOrder(HIGHEST_Z);

            pl->m_uiLayer->addChildAtPosition(f->inputMeter, Anchor::Right, {-12.5f, 0.f}, false);
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

        if (button == 1 && down) {
            if (auto pl = PlayLayer::get()) {
                auto f = m_fields.self();

                if (f->inputMeter) {  // nesting hell :D
                    if (f->inputMeter->isDeath()) {
                        pl->destroyPlayer(pl->m_player1, nullptr);
                    } else if (f->inputMeter->isDanger()) {
                        rng::flip() ? pl->destroyPlayer(pl->m_player1, nullptr) : Notification::create("Careful when you jump!", NotificationIcon::Warning, 0.5f)->show();
                    };
                };
            };
        };
    };

    InputMeter* getHealthMeter() noexcept {
        if (auto f = m_fields.self()) return f->inputMeter;
        return nullptr;
    };
};

class $modify(InputRoulettePlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    void destroyPlayer(PlayerObject* player, GameObject* object) {
        PlayLayer::destroyPlayer(player, object);

        if (m_playerDied) {
            if (auto gjbgl = GJBaseGameLayer::get()) {
                if (auto meter = modify_cast<InputRouletteGJBaseGameLayer*>(gjbgl)->getHealthMeter()) meter->playPause(false);
            };
        };
    };

    void resetLevel() {
        PlayLayer::resetLevel();

        if (auto gjbgl = GJBaseGameLayer::get()) {
            if (auto meter = modify_cast<InputRouletteGJBaseGameLayer*>(gjbgl)->getHealthMeter()) meter->playPause(true);
        };
    };

    void resetLevelFromStart() {
        PlayLayer::resetLevelFromStart();

        if (auto gjbgl = GJBaseGameLayer::get()) {
            if (auto meter = modify_cast<InputRouletteGJBaseGameLayer*>(gjbgl)->getHealthMeter()) meter->playPause(true);
        };
    };
};

HORRIBLE_TOGGLE_MODIFY(GJBaseGameLayer, InputRouletteGJBaseGameLayer);