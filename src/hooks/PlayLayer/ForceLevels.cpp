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

static bool trySwitchToLevel(PlayLayer* pl, PlayerObject* player, GameObject* killer, int levelID, int chance, int rng, std::string_view levelName, bool dontCreateObjects, bool useReplay) {
    if (rng > chance) {
        log::debug("{} jumpscare not triggered {}", levelName, chance);
        return false;
    };

    jumpscares::util::switchToLevel(pl, levelID, levelName, player, killer, dontCreateObjects, useReplay);

    return true;
};

class $modify(GriefPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID_GRIEF);

    struct Fields {
        int chance = options::getChance(oGrief->getID());

        bool dontCreateObjects = false;

        GameObject* destroyingObject = nullptr;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        jumpscares::downloadGrief(new jumpscares::util::DownloadDelegate(this, 105001928, "Grief", false, false));
    };

    void destroyPlayer(PlayerObject* p0, GameObject* p1) {
        auto f = m_fields.self();

        if (p1 == m_anticheatSpike && !p0->m_isDead) return;
        if (!f->destroyingObject) f->destroyingObject = p1;

        int rng = randng::fast();
        trySwitchToLevel(this, p0, p1, 105001928, f->chance, rng, "Grief", f->dontCreateObjects, m_useReplay);

        PlayLayer::destroyPlayer(p0, p1);
    };
};

class $modify(CongregationPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID_CONGREG);

    struct Fields {
        int chance = options::getChance(oCongreg->getID());

        bool dontCreateObjects = false;

        GameObject* destroyingObject = nullptr;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        jumpscares::downloadCongregation(new jumpscares::util::DownloadDelegate(this, 93437568, "Congregation", false, false));
    };

    void destroyPlayer(PlayerObject* p0, GameObject* p1) {
        auto f = m_fields.self();

        if (p1 == m_anticheatSpike && !p0->m_isDead) return;
        if (!f->destroyingObject) f->destroyingObject = p1;

        int rng = randng::fast();
        trySwitchToLevel(this, p0, p1, 93437568, f->chance, rng, "Congregation", f->dontCreateObjects, m_useReplay);

        PlayLayer::destroyPlayer(p0, p1);
    };
};