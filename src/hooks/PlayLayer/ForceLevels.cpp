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

static bool trySwitchToLevel(PlayLayer* pl, PlayerObject* player, GameObject* killer, jumpscares::DownloadDelegate* delegate, int chance, int rng, bool dontCreateObjects, bool useReplay) {
    if (rng > chance) {
        log::debug("{} jumpscare not triggered {}", delegate->getLevelName(), chance);
        return false;
    };

    jumpscares::switchToLevel(pl, delegate, player, killer, dontCreateObjects, useReplay);

    return true;
};

class $modify(GriefPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID_GRIEF);

    struct Fields {
        int chance = options::getChance(oGrief->getID());

        bool dontCreateObjects = false;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        jumpscares::saveLevel(jumpscares::get::grief());
    };

    void destroyPlayer(PlayerObject* p0, GameObject* p1) {
        auto f = m_fields.self();

        if (p1 == m_anticheatSpike && !p0->m_isDead) return;

        int rng = randng::fast();
        trySwitchToLevel(this, p0, p1, jumpscares::get::grief(), f->chance, rng, f->dontCreateObjects, m_useReplay);

        PlayLayer::destroyPlayer(p0, p1);
    };
};

class $modify(CongregationPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID_CONGREG);

    struct Fields {
        int chance = options::getChance(oCongreg->getID());

        bool dontCreateObjects = false;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        jumpscares::saveLevel(jumpscares::get::congregation());
    };

    void destroyPlayer(PlayerObject* p0, GameObject* p1) {
        auto f = m_fields.self();

        if (p1 == m_anticheatSpike && !p0->m_isDead) return;

        int rng = randng::fast();
        trySwitchToLevel(this, p0, p1, jumpscares::get::congregation(), f->chance, rng, f->dontCreateObjects, m_useReplay);

        PlayLayer::destroyPlayer(p0, p1);
    };
};