#include <Utils.h>

#include <ranges>

#include <menu/OptionMenuButton.h>
#include <menu/SettingV3.h>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/GJGameLevel.hpp>

#include <cheeseworks.moddevbranding/include/OptionalAPI.hpp>

using namespace horrible::prelude;

static std::vector<std::weak_ptr<Hook>> s_safeModeHooks;
static std::vector<std::weak_ptr<Hook>> s_floatingBtnHooks;

#define HORRIBLE_HOOK_INTERNAL(vectorRef)                                              \
    static void onModify(auto& self) {                                                 \
        utils::StringMap<std::shared_ptr<Hook>>& hooks = self.m_hooks;                 \
        auto enable = thisMod->getSettingValue<bool>(setting::FloatingBtn);            \
                                                                                       \
        for (auto& hook : hooks | std::views::values) {                                \
            hook->setAutoEnable(enable);                                               \
            (void)hook->toggle(enable);                                                \
                                                                                       \
            (void)self.setHookPriorityPre(hook->getDisplayName(), Priority::FirstPre); \
                                                                                       \
            vectorRef.push_back(hook);                                                 \
        };                                                                             \
    }

$on_game(Loaded) {
    (void)thisMod->registerCustomSettingType("menu", &HorribleSettingV3::parse);
    if (auto fb = OptionMenuButton::get()) OverlayManager::get()->addChild(fb);

    listenForSettingChanges<bool>(
        setting::SafeMode,
        [](bool value) {
            for (auto const& hook : s_safeModeHooks) {
                if (auto const h = hook.lock()) {
                    log::trace("Toggling safe mode hook '{}' {}...", h->getDisplayName(), value ? "ON" : "OFF");
                    (void)h->toggle(value);
                };
            };
        });

    listenForKeybindSettingPresses(
        "key-popup",
        [](Keybind const&, bool down, bool repeat, double) {
            if (down && !repeat) menu::open();
        });

    listenForSettingChanges<bool>(
        setting::FloatingBtn,
        [](bool value) {
            if (auto fb = OptionMenuButton::get()) fb->setVisible(value);

            for (auto const& hook : s_floatingBtnHooks) {
                if (auto const h = hook.lock()) {
                    log::trace("Toggling floating button hook '{}' {}...", h->getDisplayName(), value ? "ON" : "OFF");
                    (void)h->toggle(value);
                };
            };
        });

    listenForSettingChanges<bool>(
        "floating-btn-level",
        [](bool value) {
            if (auto fb = OptionMenuButton::get()) fb->setShowInLevel(value);
        });

    listenForSettingChanges<float>(
        "floating-btn-scale",
        [](float value) {
            if (auto fb = OptionMenuButton::get()) fb->setScale(value);
        });

    listenForSettingChanges<int64_t>(
        "floating-btn-opacity",
        [](int64_t value) {
            if (auto fb = OptionMenuButton::get()) fb->setOpacity(value);
        });

    listenForSettingChanges<std::string>(
        "floating-btn-icon",
        [](std::string value) {
            if (auto fb = OptionMenuButton::get()) fb->setButtonIcon(std::move(value));
        });

    listenForSettingChanges<std::string>(
        "theme",
        [](std::string value) {
            if (auto fb = OptionMenuButton::get()) fb->setTheme(std::move(value));
        });

    // TODO: host custom branding image in mod dev branding server for breakeode
    (void)branding::registerBrand(GEODE_MOD_ID, "https://github.com/CubicCommunity/HorribleMenu/blob/main/logo.png?raw=true", branding::Type::URL);
};

// safe mode
class $modify(HISafeGJGameLevel, GJGameLevel) {
    HORRIBLE_HOOK_INTERNAL(s_safeModeHooks);

    void savePercentage(int, bool, int, int, bool) {
        log::warn("Safe mode is enabled, progress will not be saved!");
    };
};

// safe mode
class $modify(HISafePlayLayer, PlayLayer) {
    HORRIBLE_HOOK_INTERNAL(s_safeModeHooks);

    // safe mode prevents level completion
    void levelComplete() {
        log::warn("Safe mode is enabled, progress will not be saved");

        bool testMode = m_isTestMode;

        m_isTestMode = true;
        PlayLayer::levelComplete();
        m_isTestMode = testMode;
    };
};

class $modify(HIFloatBtnPauseLayer, PauseLayer) {
    HORRIBLE_HOOK_INTERNAL(s_floatingBtnHooks);

    void customSetup() {
        auto toggle = thisMod->getSettingValue<bool>(setting::FloatingBtn);

        log::trace("{} floating button", toggle ? "Showing" : "Hiding");
        if (auto fb = OptionMenuButton::get()) fb->setVisible(toggle);

        PauseLayer::customSetup();
    };
};

class $modify(HIFloatBtnPlayLayer, PlayLayer) {
    HORRIBLE_HOOK_INTERNAL(s_floatingBtnHooks);

    void setupHasCompleted() {
        toggleButton();
        PlayLayer::setupHasCompleted();
    };

    void resume() {
        toggleButton();
        PlayLayer::resume();
    };

    void resumeAndRestart(bool fromStart) {
        toggleButton();
        PlayLayer::resumeAndRestart(fromStart);
    };

    void onQuit() {
        toggleButton(true);
        PlayLayer::onQuit();
    };

    void showEndLayer() {
        toggleButton(true);
        PlayLayer::showEndLayer();
    };

    void toggleButton(bool toggle = false) {
        log::trace("{} floating button", toggle ? "Showing" : "Hiding");
        if (auto fb = OptionMenuButton::get()) fb->setVisible(thisMod->getSettingValue<bool>(setting::FloatingBtn) && (fb->showInLevel() || toggle));
    };
};