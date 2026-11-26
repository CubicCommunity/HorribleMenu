#include "../Jumpscares.hpp"

// #include <dankmeme.argon/argon.hpp>

#include <Util.h>

#include <rhatt/prelude.h>

#include <asp/iter.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace cs::brkd::horrible::util;

void jumpscares::downloadLevel(int level, Callback&& callback) {
    cw::DelegateDirector::get()->downloadLevel(
        level,
        [cb = std::move(callback)](Result<Ref<GJGameLevel>> res) {
            if (res.isOk()) {
                auto lvl = std::move(res).unwrap();

                GameLevelManager::sharedState()->saveLevel(lvl);
                if (cb) cb(std::move(lvl));
            };
        }
    );
}

void jumpscares::switchLevel(int level, bool dontCreateObjects, Callback&& callback) {
    if (auto pl = PlayLayer::get()) {
        if (pl->m_level->m_levelID == level) return;
    };

    downloadLevel(
        level,
        [dontCreateObjects, cb = std::move(callback)](Ref<GJGameLevel> lvl) {
            CCDirector::sharedDirector()->replaceScene(PlayLayer::scene(lvl, dontCreateObjects));
            if (cb) cb(lvl);
        }
    );
}