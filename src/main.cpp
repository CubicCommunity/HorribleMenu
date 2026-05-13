#include <Utils.h>

#include <ranges>

#include <ui/Menu.h>
#include <ui/MenuButton.h>
#include <ui/SettingV3.h>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/GJGameLevel.hpp>

#include <cheeseworks.moddevbranding/include/OptionalAPI.hpp>

using namespace horrible::prelude;

static std::vector<std::weak_ptr<Hook>> s_safeModeHooks;
static std::vector<std::weak_ptr<Hook>> s_floatingBtnHooks;

#define HORRIBLE_HOOK_INTERNAL(hookVector, settingId)                                  \
    static void onModify(auto& self) {                                                 \
        utils::StringMap<std::shared_ptr<Hook>>& hooks = self.m_hooks;                 \
        auto enable = Mod::get()->getSettingValue<bool>(settingId);                    \
                                                                                       \
        for (auto& hook : hooks | std::views::values) {                                \
            hook->setAutoEnable(enable);                                               \
            (void)hook->toggle(enable);                                                \
                                                                                       \
            (void)self.setHookPriorityPre(hook->getDisplayName(), Priority::FirstPre); \
                                                                                       \
            hookVector.push_back(hook);                                                \
        };                                                                             \
    }

$on_game(Loaded) {
    (void)mod->registerCustomSettingType("menu", &HorribleSettingV3::parse);

    if (auto om = OverlayManager::get()) {
        if (auto fb = MenuButton::get()) om->addChild(fb);
    };

    listenForSettingChanges<bool>(
        setting::SafeMode,
        [](bool value) {
            for (auto const& hook : s_safeModeHooks) {
                if (auto h = hook.lock()) {
                    log::trace("Toggling safe mode hook '{}' {}...", h->getDisplayName(), value ? "ON" : "OFF");
                    (void)h->toggle(value);
                };
            };
        });

    listenForKeybindSettingPresses(
        "key-popup",
        [](auto, bool down, bool repeat, auto) {
            if (down && !repeat) menu::open();
        });

    listenForSettingChanges<bool>(
        setting::FloatingBtn,
        [](bool value) {
            if (auto fb = MenuButton::get()) {
                fb->setTouchEnabled(value);
                fb->setVisible(value);
            };

            for (auto const& hook : s_floatingBtnHooks) {
                if (auto h = hook.lock()) {
                    log::trace("Toggling floating button hook '{}' {}...", h->getDisplayName(), value ? "ON" : "OFF");
                    (void)h->toggle(value);
                };
            };
        });

    listenForSettingChanges<bool>(
        "floating-btn-level",
        [](bool value) {
            if (auto fb = MenuButton::get()) fb->setShowInLevel(value);
        });

    listenForSettingChanges<float>(
        "floating-btn-scale",
        [](float value) {
            if (auto fb = MenuButton::get()) fb->setScale(value);
        });

    listenForSettingChanges<uint8_t>(
        "floating-btn-opacity",
        [](uint8_t value) {
            if (auto fb = MenuButton::get()) fb->setOpacity(value);
        });

    listenForSettingChanges<std::string>(
        "floating-btn-icon",
        [](std::string value) {
            if (auto fb = MenuButton::get()) fb->setButtonIcon(std::move(value));
        });

    listenForSettingChanges<std::string>(
        "theme",
        [](std::string value) {
            if (auto fb = MenuButton::get()) fb->setTheme(std::move(value));
        });

    listenForAllHorribleOptionChanges(
        [](std::string_view id, HorribleOptionSave data) {
            log::trace("Global options listener detected {} being {}, {}, {}", id, data.enabled ? "enabled" : "disabled", data.pin ? "pinned" : "unpinned", data.viewed ? "viewed" : "not viewed yet");
        });

    (void)branding::registerBrand(GEODE_MOD_ID, "https://moddev.cheeseworks.gay/cdn/cubic_horriblemenu.webp", branding::Type::URL);
};

// safe mode
class $modify(HISafeGJGameLevel, GJGameLevel) {
    HORRIBLE_HOOK_INTERNAL(s_safeModeHooks, setting::SafeMode);

    void savePercentage(int, bool, int, int, bool) {
        log::warn("Safe mode is enabled, so progress will not be saved!");
    };
};

// safe mode
class $modify(HISafePlayLayer, PlayLayer) {
    HORRIBLE_HOOK_INTERNAL(s_safeModeHooks, setting::SafeMode);

    // safe mode prevents level completion
    void levelComplete() {
        log::warn("Safe mode is enabled, so progress will not be saved");

        bool testMode = m_isTestMode;

        m_isTestMode = true;
        PlayLayer::levelComplete();
        m_isTestMode = testMode;
    };
};

class $modify(HIFloatBtnPauseLayer, PauseLayer) {
    HORRIBLE_HOOK_INTERNAL(s_floatingBtnHooks, setting::FloatingBtn);

    void customSetup() {
        auto toggle = mod->getSettingValue<bool>(setting::FloatingBtn);

        log::trace("{} floating button", toggle ? "Showing" : "Hiding");
        if (auto fb = MenuButton::get()) {
            fb->setTouchEnabled(toggle);
            fb->setVisible(toggle);
        };

        PauseLayer::customSetup();
    };
};

class $modify(HIFloatBtnPlayLayer, PlayLayer) {
    HORRIBLE_HOOK_INTERNAL(s_floatingBtnHooks, setting::FloatingBtn);

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

        if (auto fb = MenuButton::get()) {
            auto toggleTo = mod->getSettingValue<bool>(setting::FloatingBtn) && (fb->showInLevel() || toggle);

            fb->setVisible(toggleTo);
            fb->setTouchEnabled(toggleTo);
        };
    };
};