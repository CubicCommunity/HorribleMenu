#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/LevelPage.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "placebo"

static auto const o = Option::create(THIS_ID)
                          ->setName("Placebo")
                          ->setDescription("A random chance that when you start a level, all the options you have enabled are disabled, or all the options you have disabled are enabled.\n<cl>suggested by tmdXD</c>")
                          ->setCategory(category::misc)
                          ->setSillyTier(SillyTier::High)
                          ->autoRegister();

void placeboEffect() {
    auto rnd = rng::fast();
    log::trace("placebo effect roll: {}", rnd);

    if (rnd <= 1) {  // 1% chance :trol:
        log::warn("Placebo effect activated! Toggling all options...");

        for (auto const& option : options::getAll()) {
            if (auto o = option.lock()) {
                auto saved = options::get(o->getID());
                log::debug("Placebo {} option {}", saved.enabled ? "enabled" : "disabled", o->getID());
                options::set(o->getID(), !saved.enabled, saved.pin, saved.viewed);
            };
        };
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