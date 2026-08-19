#include <Util.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "size_changer"

static auto const o = Option::create(THIS_ID)
                          ->setName("Size Changer")
                          ->setDescription("Randomly change the player size every time you jump.\n<cl>suggested by himynameisryan21</c>")
                          ->setCategory(category::randoms)
                          ->setSillyTier(SillyTier::Medium)
                          ->setCheating(true)
                          ->autoRegister();

class $modify(SizeChangerPlayerObject, PlayerObject) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        uint8_t chance = options::getChance(THIS_ID);

        bool scaled = false;
    };

    bool pushButton(PlayerButton p0) {
        if (!m_gameLayer) return PlayerObject::pushButton(p0);

        auto f = m_fields.self();

        if (rng::chance(f->chance)) {
            togglePlayerScale(!f->scaled, false);
            f->scaled = !f->scaled;
        };

        return PlayerObject::pushButton(p0);
    };
};