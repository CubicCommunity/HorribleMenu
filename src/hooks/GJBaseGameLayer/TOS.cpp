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
        Ref<TermsAndConditions> currentTos = nullptr;
    };

    void handleButton(bool down, int button, bool isPlayer1) {
        GJBaseGameLayer::handleButton(down, button, isPlayer1);

        if (down) {
            auto f = m_fields.self();

            if (f->currentTos) {
                f->currentTos.take()->removeFromParent();
                f->currentTos = nullptr;
            } else if (auto popup = TermsAndConditions::create(
                           [this](bool accepted) {
                               if (!accepted) {
                                   Notification::create("You declined the terms and conditions!", NotificationIcon::Error)->show();
                                   if (auto pl = PlayLayer::get()) pl->resetLevelFromStart();
                               };
                           })) {
                popup->show();
                f->currentTos = popup;
            };
        };
    };
};