#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "terms_and_conditions"

static auto const o = Option::create(THIS_ID)
                          ->setName("Agree To Our Terms!")
                          ->setDescription("Whenever you try to jump, change gamemode, or interact with anything in the level, you must agree with the terms and conditions to continue playing.\n<cl>suggested by ArcticWoof</c>")
                          ->setCategory(category::obstructive)
                          ->setSillyTier(SillyTier::Medium)
                          ->autoRegister();

class $modify(TOSGJBaseGameLayer, GJBaseGameLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields {
        uint8_t chance = options::getChance(THIS_ID);

        Ref<TermsAndConditions> currentTos = nullptr;
    };

    void handleButton(bool down, int button, bool isPlayer1) {
        GJBaseGameLayer::handleButton(down, button, isPlayer1);

        if (!down || button != 1) return;

        auto f = m_fields.self();

        if (randng::fast() <= f->chance) {
            cue::resetNode(f->currentTos);

            if (auto popup = TermsAndConditions::create(
                    [this](bool accepted) {
                        cursor::hide();
                        updateTimeWarp(1.f);

                        if (!accepted) {
                            Notification::create("You declined our terms and conditions!", NotificationIcon::Error)->show();

                            sfx::play(sfx::file::bad);
                            if (auto pl = PlayLayer::get()) pl->resetLevelFromStart();
                        } else {
                            sfx::play(sfx::file::good);
                        };
                    })) {
                popup->show();
                f->currentTos = popup;

                updateTimeWarp(0.125f);
            };
        };
    };
};