#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/LevelPage.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

static constexpr auto id = "placebo";

static auto const o = Option::create(id)
                          .setName("Placebo")
                          .setDescription("A random chance that when you start a level, all the options you have enabled are disabled, or all the options you have disabled are enabled.\n<cl>suggested by tmdXD</c>")
                          .setCategory(category::misc)
                          .setSillyTier(SillyTier::High);
HORRIBLE_REGISTER_OPTION(o);

void placeboEffect() {
    int rnd = randng::fast();
    log::debug("placebo effect roll: {}", rnd);

    if (rnd <= 1) {  // 1% chance :trol:
        log::warn("Placebo effect activated! Toggling all options...");

        for (auto const& option : options::getAll()) {
            auto saved = options::get(option.getID());
            log::debug("Placebo {} option {}", saved.enabled ? "enabled" : "disabled", option.getID());
            options::set(option.getID(), !saved.enabled, saved.pin, saved.viewed);
        };
    };
};

class $modify(PlaceboLevelPage, LevelPage) {
    HORRIBLE_DELEGATE_HOOKS(id);

    void onPlay(CCObject* sender) {
        placeboEffect();
        log::trace("Placebo triggered in level page");

        LevelPage::onPlay(sender);
    };
};

class $modify(PlaceboLevelInfoLayer, LevelInfoLayer) {
    HORRIBLE_DELEGATE_HOOKS(id);

    void onPlay(CCObject* sender) {
        placeboEffect();
        log::trace("Placebo triggered in level info layer");

        LevelInfoLayer::onPlay(sender);
    };
};