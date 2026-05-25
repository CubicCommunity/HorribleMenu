#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID_GRIEF "grief"
#define THIS_ID_CONGREG "congregation"

static auto const oGrief = Option::create(THIS_ID_GRIEF)
                               ->setName("Get Back on Grief")
                               ->setDescription("A chance of forcing you to play Grief when you die in a level.\n<cl>suggested by Sweep</c>")
                               ->setCategory(category::jumpscares)
                               ->setSillyTier(SillyTier::High)
                               ->setOnline(true)
                               ->setCheating(true)
                               ->autoRegister();

static auto const oCongreg = Option::create(THIS_ID_CONGREG)
                                 ->setName("Congregation Jumpscare")
                                 ->setDescription("A chance of forcing you to play Congregation when you die in a level.\n<cl>suggested by StaticGD</c>")
                                 ->setCategory(category::jumpscares)
                                 ->setSillyTier(SillyTier::High)
                                 ->setOnline(true)
                                 ->setCheating(true)
                                 ->autoRegister();

static void trySwitchToLevel(jumpscares::LevelInfo const& level, uint8_t chance, uint8_t rng, bool useReplay) {
    if (rng > chance) {
        log::trace("jumpscare not triggered with {}/100 chance", chance);
        return;
    };

    log::debug("jumpscare triggered!");
    jumpscares::switchLevel(level, false, useReplay);
};

class $modify(GriefPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID_GRIEF);

    struct Fields final {
        uint8_t chance = options::getChance(oGrief->getID());
    };

    void destroyPlayer(PlayerObject* player, GameObject* object) {
        PlayLayer::destroyPlayer(player, object);

        if (object == m_anticheatSpike && !player->m_isDead) return;

        trySwitchToLevel(jumpscares::level::grief, m_fields->chance, rng::fast(), m_useReplay);
    };
};

class $modify(CongregationPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID_CONGREG);

    struct Fields final {
        uint8_t chance = options::getChance(oCongreg->getID());
    };

    void destroyPlayer(PlayerObject* player, GameObject* object) {
        PlayLayer::destroyPlayer(player, object);

        if (options::isEnabled(THIS_ID_GRIEF)) return;

        if (object == m_anticheatSpike && !player->m_isDead) return;

        trySwitchToLevel(jumpscares::level::congregation, m_fields->chance, rng::fast(), m_useReplay);
    };
};