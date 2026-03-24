#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/LevelPage.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

static constexpr auto id = "placebo";

static auto const o = Option::create(id)
                          .setName("Placebo")
                          .setDescription("A random chance that when you start a level, all the options you have enabled are disabled, or all the options you have disabled are enabled.\n<cl>Credit: tmdXD</c>")
                          .setCategory(category::misc)
                          .setSillyTier(SillyTier::High);
HORRIBLE_REGISTER_OPTION(o);

void placeboEffect() {
    log::info("Checking for placebo effect...");

    int rnd = randng::fast();
    log::info("placebo effect roll: {}", rnd);

    if (rnd <= 1) {  // 1% chance :trol:
        log::info("Placebo effect activated! Toggling all options...");

        for (auto const& option : options::getAll()) {
            auto toggle = options::isEnabled(option.getID());
            log::debug("Placebo {} option {}", toggle ? "disabled" : "enabled", option.getID());
            options::set(option.getID(), !toggle);
        };
    };
};

class $modify(PlaceboLevelPage, LevelPage) {
    HORRIBLE_DELEGATE_HOOKS(id);

    void onPlay(CCObject* sender) {
        placeboEffect();
        log::debug("Placebo triggered in level page");

        LevelPage::onPlay(sender);
    };
};

class $modify(PlaceboLevelInfoLayer, LevelInfoLayer) {
    HORRIBLE_DELEGATE_HOOKS(id);

    void onPlay(CCObject* sender) {
        placeboEffect();
        log::debug("Placebo triggered in level info layer");

        LevelInfoLayer::onPlay(sender);
    };
};