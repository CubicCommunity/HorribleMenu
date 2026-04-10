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
                               ->setOnline(true);
HORRIBLE_REGISTER_OPTION(oGrief);

static auto const oCongreg = Option::create(THIS_ID_CONGREG)
                                 ->setName("Congregation Jumpscare")
                                 ->setDescription("A chance at death of forcing you to play Congregation.\n<cl>suggested by StaticGD</c>")
                                 ->setCategory(category::jumpscares)
                                 ->setSillyTier(SillyTier::High)
                                 ->setOnline(true);
HORRIBLE_REGISTER_OPTION(oCongreg);

static bool isAlreadyInLevel(PlayLayer* pl, int levelID) {
    return pl->m_level && pl->m_level->m_levelID.value() == levelID;
};

static bool trySwitchToLevel(PlayLayer* pl, PlayerObject* player, GameObject* killer, int levelID, int chance, int rng, std::string_view levelName, bool dontCreateObjects, bool useReplay) {
    if (rng > chance) {
        log::info("{} jumpscare not triggered {}", levelName, chance);
        return false;
    };

    auto glm = GameLevelManager::get();
    auto targetLevel = glm->getSavedLevel(levelID);

    if (!targetLevel || targetLevel->m_levelNotDownloaded) return false;

    if (isAlreadyInLevel(pl, levelID)) {
        log::debug("Already in {} level", levelName);
        return false;
    };

    pl->PlayLayer::destroyPlayer(player, killer);
    pl->onExit();

    auto scene = PlayLayer::scene(targetLevel, useReplay, dontCreateObjects);
    CCDirector::get()->replaceScene(scene);

    log::info("Switching to {} level ({})", levelName, levelID);

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
        jumpscares::downloadGrief();
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
        jumpscares::downloadCongregation();
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