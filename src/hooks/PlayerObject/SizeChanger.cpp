#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class $modify(SizeChangerPlayerObject, PlayerObject) {
    struct Fields {
        bool enabled = options::get("size_changer");
        int chance = options::getChance("size_changer");

        bool scaled = false;
    };

    bool pushButton(PlayerButton button) {
        auto f = m_fields.self();

        if (auto pl = PlayLayer::get()) {
            if (f->enabled) {
                // log::debug("size changer jump detected");
                int rnd = randng::fast();
                if (rnd <= f->chance) {
                    if (f->scaled == true) {
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