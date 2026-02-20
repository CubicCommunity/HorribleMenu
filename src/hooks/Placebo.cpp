#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/LevelPage.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static Option const o = {
    "placebo",
    "Placebo",
    "A random chance that when you start a level, all the options you have enabled are disabled, or all the options you have disabled are enabled.\n<cy>Credit: tmdXD</c>",
    category::misc,
    SillyTier::High,
};
REGISTER_HORRIBLE_OPTION(o);

void placeboEffect() {
    log::info("Checking for placebo effect...");

    if (options::get(o.id)) {
        int rnd = randng::fast();
        log::info("placebo effect roll: {}", rnd);

        if (rnd <= 1) { // 1% chance :trol:
            log::info("Placebo effect activated! Toggling all options...");

            for (auto const& option : options::getAll()) {
                auto toggle = options::get(option.id);
                log::debug("Placebo {} option {}", toggle ? "disabled" : "enabled", option.id);
                options::set(option.id, !toggle);
            };
        };
    };
};

class $modify(PlaceboLevelPage, LevelPage) {
    void onPlay(CCObject * sender) {
        placeboEffect();
        log::debug("Placebo triggered in level page");

        LevelPage::onPlay(sender);
    };
};

class $modify(PlaceboLevelInfoLayer, LevelInfoLayer) {
    void onPlay(CCObject * sender) {
        placeboEffect();
        log::debug("Placebo triggered in level info layer");

        LevelInfoLayer::onPlay(sender);
    };
};