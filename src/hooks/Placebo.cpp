#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/LevelPage.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static constexpr auto id = "placebo";

inline static Option const o = {
    id,
    "Placebo",
    "A random chance that when you start a level, all the options you have enabled are disabled, or all the options you have disabled are enabled.\n<cy>Credit: tmdXD</c>",
    category::misc,
    SillyTier::High,
};
HORRIBLE_REGISTER_OPTION(o);

void placeboEffect() {
    log::info("Checking for placebo effect...");

    int rnd = randng::fast();
    log::info("placebo effect roll: {}", rnd);

    if (rnd <= 1) { // 1% chance :trol:
        log::info("Placebo effect activated! Toggling all options...");

        for (auto const& option : options::getAll()) {
            auto toggle = options::isEnabled(option.id);
            log::debug("Placebo {} option {}", toggle ? "disabled" : "enabled", option.id);
            options::set(option.id, !toggle);
        };
    };
};

class $modify(PlaceboLevelPage, LevelPage) {
    HORRIBLE_DELEGATE_HOOKS(id);

    void onPlay(CCObject * sender) {
        placeboEffect();
        log::debug("Placebo triggered in level page");

        LevelPage::onPlay(sender);
    };
};

class $modify(PlaceboLevelInfoLayer, LevelInfoLayer) {
    HORRIBLE_DELEGATE_HOOKS(id);

    void onPlay(CCObject * sender) {
        placeboEffect();
        log::debug("Placebo triggered in level info layer");

        LevelInfoLayer::onPlay(sender);
    };
};