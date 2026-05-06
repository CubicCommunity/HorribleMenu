#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "random_icon"

static auto const o = Option::create(THIS_ID)
                          ->setName("Random Icon Change")
                          ->setDescription("Randomly change your icon every time you jump.\n<cl>suggested by JompyDoJump</c>")
                          ->setCategory(category::randoms)
                          ->setSillyTier(SillyTier::Low)
                          ->autoRegister();

class $modify(RandomIconPlayerObject, PlayerObject) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    bool pushButton(PlayerButton p0) {
        // jump pls
        if (m_gameLayer && p0 == PlayerButton::Jump) {
            auto gm = GameManager::sharedState();
            auto rnd = randng::tiny();

            // count the icons i guess
            auto maxIcons = 0;
            if (gm) maxIcons = gm->countForType(IconType::Cube);
            if (maxIcons <= 0) maxIcons = 38;

            // pick random icons that is unlocked
            auto tries = 0;
            auto newIcon = rnd % maxIcons + 1;

            while (tries < 20 && gm && !gm->isIconUnlocked(newIcon, IconType::Cube)) {
                newIcon = rnd % maxIcons + 1;
                tries++;
            };

            // randomize the colors of the icon
            auto r = randng::get(256);
            auto g = randng::get(256);
            auto b = randng::get(256);

            setColor(ccc3(r, g, b));
            updatePlayerGlow();

            updatePlayerFrame(newIcon);
            log::debug("Changed player icon to {}", newIcon);
        };

        return PlayerObject::pushButton(p0);
    };
};