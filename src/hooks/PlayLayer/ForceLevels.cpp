#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

static constexpr auto idGrief = "grief";
static constexpr auto idCongregation = "congregation";

static auto const oGrief = Option::create(idGrief)
                               .setName("Get Back on Grief")
                               .setDescription("A chance at death of forcing you to play Grief.\n<cl>suggested by Sweep</c>")
                               .setCategory(category::jumpscares)
                               .setSillyTier(SillyTier::High);
HORRIBLE_REGISTER_OPTION(oGrief);

static auto const oCongregation = Option::create(idCongregation)
                                      .setName("Congregation Jumpscare")
                                      .setDescription("A chance at death of forcing you to play Congregation.\n<cl>suggested by StaticGD</c>")
                                      .setCategory(category::jumpscares)
                                      .setSillyTier(SillyTier::High);
HORRIBLE_REGISTER_OPTION(oCongregation);

static bool isAlreadyInLevel(PlayLayer* layer, int levelID) {
    return layer->m_level && layer->m_level->m_levelID.value() == levelID;
};

static bool trySwitchToLevel(PlayLayer* layer, PlayerObject* player, GameObject* killer, int levelID, int chance, int rng, std::string_view levelName, bool dontCreateObjects, bool useReplay) {
    if (rng > chance) {
        log::info("{} jumpscare not triggered {}", levelName, chance);
        return false;
    };

    auto glm = GameLevelManager::get();
    auto targetLevel = glm->getSavedLevel(levelID);

    if (!targetLevel || targetLevel->m_levelNotDownloaded) return false;

    if (isAlreadyInLevel(layer, levelID)) {
        log::debug("Already in {} level", levelName);
        return false;
    };

    layer->PlayLayer::destroyPlayer(player, killer);
    layer->onExit();

    auto scene = PlayLayer::scene(targetLevel, useReplay, dontCreateObjects);
    CCDirector::get()->replaceScene(scene);

    log::info("Switching to {} level ({})", levelName, levelID);

    return true;
};

class $modify(GriefPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(idGrief);

    struct Fields {
        int chance = options::getChance(oGrief.getID());

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
    HORRIBLE_DELEGATE_HOOKS(idCongregation);

    struct Fields {
        int chance = options::getChance(oCongregation.getID());

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