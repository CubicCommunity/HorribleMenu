#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static Option const o = {
        "random_icon",
        "Random Icon Change",
        "Randomly change your icon every time you jump.\n<cy>Credit: JompyDoJump</c>",
        category::randoms,
        SillyTier::Low,
};
REGISTER_HORRIBLE_OPTION(o);

class $modify(RandomIconPlayerObject, PlayerObject) {
    struct Fields {
        bool enabled = options::get(o.id);
    };

    bool pushButton(PlayerButton p0) {
        if (m_fields->enabled) {
            // jump pls
            if (m_gameLayer && p0 == PlayerButton::Jump) {
                auto gm = GameManager::sharedState();
                int rnd = randng::tiny();

                // count the icons i guess
                int maxIcons = 0;
                if (gm) maxIcons = gm->countForType(IconType::Cube);
                if (maxIcons <= 0) maxIcons = 38;

                // pick random icons that is unlocked
                int tries = 0;
                int newIcon = rnd % maxIcons + 1;

                while (tries < 20 && gm && !gm->isIconUnlocked(newIcon, IconType::Cube)) {
                    newIcon = rnd % maxIcons + 1;
                    tries++;
                };

                // randomize the colors of the icon
                int r = randng::get(256);
                int g = randng::get(256);
                int b = randng::get(256);

                setColor(ccc3(r, g, b));
                updatePlayerGlow();

                updatePlayerFrame(newIcon);
                log::debug("Changed player icon to {}", newIcon);
            };
        };

        return PlayerObject::pushButton(p0);
    };
};