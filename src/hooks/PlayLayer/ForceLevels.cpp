#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

static auto const oGrief = Option::create("grief")
                               .setName("Get Back on Grief")
                               .setDescription("A chance at death of forcing you to play Grief.\n<cl>Credit: Sweep</c>")
                               .setCategory(category::jumpscares)
                               .setSillyTier(SillyTier::High);
HORRIBLE_REGISTER_OPTION(oGrief);

static auto const oCongregation = Option::create("congregation")
                                      .setName("Congregation Jumpscare")
                                      .setDescription("A chance at death of forcing you to play Congregation.\n<cl>Credit: StaticGD</c>")
                                      .setCategory(category::jumpscares)
                                      .setSillyTier(SillyTier::High);
HORRIBLE_REGISTER_OPTION(oCongregation);

class $modify(ForceLevelsPlayLayer, PlayLayer) {
    struct Fields {
        bool griefEnabled = options::isEnabled(oGrief.getID());
        bool congregEnabled = options::isEnabled(oCongregation.getID());

        int griefChance = options::getChance(oGrief.getID());
        int congregChance = options::getChance(oCongregation.getID());

        bool m_dontCreateObjects = false;
        GameObject* m_destroyingObject;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        return true;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        auto f = m_fields.self();

        if (f->griefEnabled) jumpscares::downloadGrief();
        if (f->congregEnabled) jumpscares::downloadCongregation();
    };

    void destroyPlayer(PlayerObject* p0, GameObject* p1) {
        auto f = m_fields.self();

        int rng = randng::fast();

        // get back to grief
        if (f->griefEnabled) {
            if (p1 == m_anticheatSpike && !p0->m_isDead) return;
            if (!f->m_destroyingObject) f->m_destroyingObject = p1;

            // chance to play grief level
            if (rng <= f->griefChance) {
                auto glm = GameLevelManager::get();
                auto griefLevel = glm->getSavedLevel(105001928);

                if (griefLevel && !griefLevel->m_levelNotDownloaded && (!m_level || m_level->m_levelID.value() != 105001928)) {
                    PlayLayer::destroyPlayer(p0, p1);

                    onExit();

                    auto scene = PlayLayer::scene(griefLevel, m_useReplay, f->m_dontCreateObjects);

                    CCDirector::get()->replaceScene(scene);
                    log::info("Switching to Grief level (105001928)");
                } else if (griefLevel && !griefLevel->m_levelNotDownloaded) {
                    log::debug("Already in grief level");
                };
            } else {
                log::info("Grief jumpscare not triggered {}", f->griefChance);
            };
        };

        // congregation jumpscare
        if (f->congregEnabled) {
            // 10% chance to play congregation level
            if (rng <= f->congregChance) {
                auto glm = GameLevelManager::get();
                auto congregLevel = glm->getSavedLevel(93437568);

                if (congregLevel && !congregLevel->m_levelNotDownloaded && (!m_level || m_level->m_levelID.value() != 93437568)) {
                    PlayLayer::destroyPlayer(p0, p1);

                    onExit();

                    auto scene = PlayLayer::scene(congregLevel, m_useReplay, f->m_dontCreateObjects);

                    CCDirector::get()->replaceScene(scene);
                    log::info("Switching to Congregation level (93437568)");
                } else if (congregLevel && !congregLevel->m_levelNotDownloaded) {
                    log::debug("Already in congregation level");
                };
            } else {
                log::info("Congregation jumpscare not triggered {}", f->congregChance);
            };
        };

        PlayLayer::destroyPlayer(p0, p1);
    };
};