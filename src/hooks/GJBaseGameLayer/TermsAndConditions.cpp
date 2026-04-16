#include <Utils.h>
#include <util/ui/TermsAndCondition.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;
using namespace horrible::ui;

#define THIS_ID "terms_and_conditions"

static auto const o = Option::create(THIS_ID)
                          ->setName("Terms and Conditions")
                          ->setDescription("When you try to jump, change gamemode, or interact with anything within the level. You must agree with the terms and conditions to continue.\n<cl>suggested by ArcticWoof</c>")
                          ->setCategory(category::obstructive)
                          ->setSillyTier(SillyTier::Medium);
HORRIBLE_REGISTER_OPTION(o);

class $modify(TTOSGJBaseGameLayer, GJBaseGameLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    void handleButton(bool down, int button, bool isPlayer1) {
        auto popup = TermsAndCondition::create();
        if (popup) {
            popup->show();
        };
        GJBaseGameLayer::handleButton(down, button, isPlayer1);
    }
};