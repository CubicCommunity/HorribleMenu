#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "super_jump"

static auto const o = Option::create(THIS_ID)
                          ->setName("Super Jump")
                          ->setDescription("A chance for your jump input to make your player jump much higher than expected if jumping from the ground. Affects every game mode.\n<cl>created by Cheeseworks</c>")
                          ->setSillyTier(SillyTier::Medium)
                          ->setCategory(category::randoms)
                          ->setCheating(true)
                          ->autoRegister();

class $modify(SuperJumpPlayerObject, PlayerObject) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        uint8_t chance = options::getChance(THIS_ID);
    };

    bool pushButton(PlayerButton button) {
        if (button != PlayerButton::Jump || !m_gameLayer) return PlayerObject::pushButton(button);

        if (m_isOnGround && rng::chance(m_fields->chance)) {
            sfx::play(sfx::file::pop);
            boostPlayer(rng::get(25.f, 17.5f) * (m_isUpsideDown ? -1.f : 1.f));
            Notification::create("Super jump!", NotificationIcon::None, 0.125f)->show();
        } else {
            return PlayerObject::pushButton(button);
        };

        return true;
    };
};