#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static constexpr auto id = "size_changer";

inline static Option const o = {
    id,
    "Size Changer",
    "Randomly change the player size every time you jump.\n<cy>Credit: himynameisryan21</c>",
    category::randoms,
    SillyTier::Medium,
};
HORRIBLE_REGISTER_OPTION(o);

class $modify(SizeChangerPlayerObject, PlayerObject) {
    HORRIBLE_DELEGATE_HOOKS(id);

    struct Fields {
        int chance = options::getChance(id);

        bool scaled = false;
    };

    bool pushButton(PlayerButton button) {
        if (m_gameLayer) {
            auto f = m_fields.self();

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

        return PlayerObject::pushButton(button);
    };
};