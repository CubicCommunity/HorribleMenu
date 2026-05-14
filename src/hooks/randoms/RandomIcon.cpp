#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "random_icon"

static auto const o = Option::create(THIS_ID)
                          ->setName("Random Icon Change")
                          ->setDescription("Chance to randomly change your icon whenever you jump.\n<cl>suggested by JompyDoJump</c>")
                          ->setCategory(category::randoms)
                          ->setSillyTier(SillyTier::Low)
                          ->autoRegister();

class $modify(RandomIconPlayerObject, PlayerObject) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    bool pushButton(PlayerButton p0) {
        if (p0 != PlayerButton::Jump || !m_gameLayer) return PlayerObject::pushButton(p0);

        // jump pls
        if (auto gm = GameManager::sharedState()) {
            if (rng::flip()) {
                // count the icons i guess
                auto maxIcons = 0;
                maxIcons = gm->countForType(IconType::Cube);
                if (maxIcons <= 0) maxIcons = 38;

                auto newIcon = rng::get(maxIcons);

#define RANDOM_COLOR {rng::get<GLubyte>(255), rng::get<GLubyte>(255), rng::get<GLubyte>(255)}

                setColor(RANDOM_COLOR);
                setSecondColor(RANDOM_COLOR);
                setGlowColor(RANDOM_COLOR);

                updatePlayerGlow();
                updatePlayerFrame(newIcon);

                log::debug("Changed player icon to {}", newIcon);
            };
        };

        return PlayerObject::pushButton(p0);
    };
};