#include <Utils.hpp>

#include <menu/OptionMenuButton.hpp>
#include <menu/SettingV3.hpp>

#include <ranges>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/GJGameLevel.hpp>

using namespace horrible::prelude;

inline static std::vector<Hook*> safeModeHooks;
inline static std::vector<Hook*> floatingBtnHooks;

#define HORRIBLE_HOOK_SAFEMODE(hookName)                                       \
    static void onModify(auto& self) {                                         \
        Result<Hook*> hookRes = self.getHook(hookName);                        \
                                                                               \
        if (auto hook = hookRes.unwrap()) {                                    \
            auto safe = horribleMod->getSettingValue<bool>(setting::SafeMode); \
                                                                               \
            hook->setAutoEnable(safe);                                         \
            (void)hook->toggle(safe);                                          \
                                                                               \
            safeModeHooks.push_back(hook);                                     \
        };                                                                     \
    }

#define HORRIBLE_HOOK_FLOATINGBTN                                               \
    static void onModify(auto& self) {                                          \
        utils::StringMap<std::shared_ptr<Hook>>& hooks = self.m_hooks;          \
        auto enable = horribleMod->getSettingValue<bool>(setting::FloatingBtn); \
                                                                                \
        for (auto& hook : hooks | std::views::values) {                         \
            (void)hook->toggle(enable);                                         \
            hook->setAutoEnable(enable);                                        \
                                                                                \
            floatingBtnHooks.push_back(hook.get());                             \
        };                                                                      \
    }

$on_game(Loaded) {
    listenForSettingChanges<bool>(
        setting::SafeMode,
        [](bool value) {
            for (auto& hook : safeModeHooks) {
                log::trace("Toggling safe mode hook '{}' {}...", hook->getDisplayName(), value ? "ON" : "OFF");
                (void)hook->toggle(value);
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

            for (auto& hook : floatingBtnHooks) {
                log::trace("Toggling floating button hook '{}' {}...", hook->getDisplayName(), value ? "ON" : "OFF");
                (void)hook->toggle(value);
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
        "theme",
        [](std::string value) {
            if (auto fb = OptionMenuButton::get()) fb->setTheme(std::move(value));
        });

    (void)horribleMod->registerCustomSettingType("menu", &HorribleSettingV3::parse);

    if (auto fb = OptionMenuButton::get()) OverlayManager::get()->addChild(fb);
};

// safe mode
class $modify(HISafeGJGameLevel, GJGameLevel) {
    HORRIBLE_HOOK_SAFEMODE("GJGameLevel::savePercentage");

    void savePercentage(int, bool, int, int, bool) {
        log::warn("Safe mode is enabled, your progress will not be saved!");
    };
};

// safe mode
class $modify(HISafePlayLayer, PlayLayer) {
    HORRIBLE_HOOK_SAFEMODE("PlayLayer::levelComplete");

    // safe mode prevents level completion
    void levelComplete() {
        log::info("Safe mode is enabled");

        bool testMode = m_isTestMode;

        m_isTestMode = true;
        PlayLayer::levelComplete();
        m_isTestMode = testMode;
    };
};

class $modify(HIFloatBtnPauseLayer, PauseLayer) {
    HORRIBLE_HOOK_FLOATINGBTN;

    void customSetup() {
        auto toggle = horribleMod->getSettingValue<bool>(setting::FloatingBtn);

        log::trace("{} floating button", toggle ? "Showing" : "Hiding");
        if (auto fb = OptionMenuButton::get()) fb->setVisible(toggle);

        PauseLayer::customSetup();
    };
};

class $modify(HIFloatBtnPlayLayer, PlayLayer) {
    HORRIBLE_HOOK_FLOATINGBTN;

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
        if (auto fb = OptionMenuButton::get()) fb->setVisible(horribleMod->getSettingValue<bool>(setting::FloatingBtn) && (fb->showInLevel() || toggle));
    };
};