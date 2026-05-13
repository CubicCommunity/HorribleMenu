#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID_GRIEF "grief"
#define THIS_ID_CONGREG "congregation"

static auto const oGrief = Option::create(THIS_ID_GRIEF)
                               ->setName("Get Back on Grief")
                               ->setDescription("A chance at death of forcing you to play Grief.\n<cl>suggested by Sweep</c>")
                               ->setCategory(category::jumpscares)
                               ->setSillyTier(SillyTier::High)
                               ->setOnline(true)
                               ->autoRegister();

static auto const oCongreg = Option::create(THIS_ID_CONGREG)
                                 ->setName("Congregation Jumpscare")
                                 ->setDescription("A chance at death of forcing you to play Congregation.\n<cl>suggested by StaticGD</c>")
                                 ->setCategory(category::jumpscares)
                                 ->setSillyTier(SillyTier::High)
                                 ->setOnline(true)
                                 ->autoRegister();

static bool trySwitchToLevel(PlayLayer* pl, std::shared_ptr<jumpscares::DownloadDelegate> delegate, int chance, int rng, bool useReplay) {
    if (rng > chance) {
        log::debug("{} jumpscare not triggered {}", delegate->getLevelName(), chance);
        return false;
    };

    jumpscares::switchToLevel(pl, delegate, false, useReplay);

    return true;
};

class $modify(GriefPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID_GRIEF);

    struct Fields final {
        uint8_t chance = options::getChance(oGrief->getID());
    };

    void destroyPlayer(PlayerObject* p0, GameObject* p1) {
        PlayLayer::destroyPlayer(p0, p1);

        if (p1 == m_anticheatSpike && !p0->m_isDead) return;

        int rng = rng::fast();
        trySwitchToLevel(this, jumpscares::get::grief(), m_fields->chance, rng, m_useReplay);
    };
};

class $modify(CongregationPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID_CONGREG);

    struct Fields final {
        uint8_t chance = options::getChance(oCongreg->getID());
    };

    void destroyPlayer(PlayerObject* p0, GameObject* p1) {
        PlayLayer::destroyPlayer(p0, p1);

        if (p1 == m_anticheatSpike && !p0->m_isDead) return;

        int rng = rng::fast();
        trySwitchToLevel(this, jumpscares::get::congregation(), m_fields->chance, rng, m_useReplay);
    };
};