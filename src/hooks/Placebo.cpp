#include <Util.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/LevelPage.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "placebo"

static auto const o = Option::create(THIS_ID)
                          ->setName("Placebo")
                          ->setDescription("A <cc>1%</c> chance that when you start a level, all of your options get toggled to the opposite of their current state.\n<cl>suggested by tmdXD</c>")
                          ->setCategory(category::misc)
                          ->setSillyTier(SillyTier::High)
                          ->setCheating(true)
                          ->autoRegister();

void placeboEffect() {
    if (rng::fast() <= 1) {  // 1% chance :trol:
        log::warn("Placebo effect activated! Toggling all options...");

        for (auto const& option : options::getAll()) {
            if (auto o = option.lock()) {
                auto saved = options::get(o->getID());
                log::debug("Placebo {} option {}", saved.enabled ? "enabled" : "disabled", o->getID());
                options::set(o->getID(), !saved.enabled, saved.pin, saved.viewed);
            };
        };
    } else {
        log::trace("Placebo effect did not activate");
    };
};

class $modify(PlaceboLevelPage, LevelPage) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    void onPlay(CCObject* sender) {
        placeboEffect();
        log::trace("Placebo triggered in level page");

        LevelPage::onPlay(sender);
    };
};

class $modify(PlaceboLevelInfoLayer, LevelInfoLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    void onPlay(CCObject* sender) {
        placeboEffect();
        log::trace("Placebo triggered in level info layer");

        LevelInfoLayer::onPlay(sender);
    };
};