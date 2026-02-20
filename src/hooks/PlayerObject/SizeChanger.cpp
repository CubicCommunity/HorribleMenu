#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static Option const o = {
    "size_changer",
    "Size Changer",
    "Randomly change the player size every time you jump.\n<cy>Credit: himynameisryan21</c>",
    category::randoms,
    SillyTier::Medium,
};
REGISTER_HORRIBLE_OPTION(o);

class $modify(SizeChangerPlayerObject, PlayerObject) {
    struct Fields {
        bool enabled = options::get(o.id);
        int chance = options::getChance(o.id);

        bool scaled = false;
    };

    bool pushButton(PlayerButton button) {
        if (m_gameLayer) {
            auto f = m_fields.self();

            if (f->enabled) {
                // log::debug("size changer jump detected");

                if (randng::fast() <= f->chance) {
                    if (f->scaled) {
                        log::debug("change scale big");
                        togglePlayerScale(f->scaled, false);
                        f->scaled = false;
                    } else {
                        log::debug("change scale small");
                        togglePlayerScale(f->scaled, false);
                        f->scaled = true;
                    };
                };
            };
        };

        return PlayerObject::pushButton(button);
    };
};