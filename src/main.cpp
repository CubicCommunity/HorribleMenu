#include <Utils.h>

#include <ranges>

#include <ui/Menu.h>
#include <ui/MenuButton.h>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/GJGameLevel.hpp>

#include <cheeseworks.moddevbranding/include/OptionalAPI.hpp>

using namespace horrible::prelude;

static std::vector<std::weak_ptr<Hook>> g_safeModeHooks;
static std::vector<std::weak_ptr<Hook>> g_floatingBtnHooks;

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

static void toggleSafeModeHooks(bool value) {
    for (auto const& hook : g_safeModeHooks) {
        if (auto h = hook.lock()) {
            log::trace("Toggling safe mode hook '{}' {}...", h->getDisplayName(), value ? "ON" : "OFF");
            (void)h->toggle(value);
        };
    };
};

$on_game(Loaded) {
    if (auto om = OverlayManager::get()) {
        if (auto fb = MenuButton::get()) om->addChild(fb);
    };

    listenForSettingChanges<bool>(
        setting::SafeMode,
        [](bool value) {
            if (!mod->getSettingValue<bool>(setting::DynamicSafeMode)) toggleSafeModeHooks(value);
        });

    listenForSettingChanges<bool>(
        setting::DynamicSafeMode,
        [](bool) {
            if (auto om = OptionManager::get()) toggleSafeModeHooks(om->shouldBeSafeMode());
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

            for (auto const& hook : g_floatingBtnHooks) {
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

    listenForSettingChanges<bool>(
        "floating-btn-editor",
        [](bool value) {
            if (auto fb = MenuButton::get()) fb->setShowInEditor(value);
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

    ButtonSettingPressedEventV3(mod, "btn-popup")
        .listen([](std::string_view buttonKey) {
            menu::open();
        })
        .leak();

    ButtonSettingPressedEventV3(mod, "cheats")
        .listen([](std::string_view buttonKey) {
            createQuickPopup(
                "Disable All Cheats",
                "Are you sure you want to <cr>disable all options marked as cheats</c>?",
                "Cancel",
                "Yes",
                [](auto, bool ok) {
                    if (ok) {
                        for (auto const& option : OptionManager::get()->getOptions()) {
                            if (auto o = option.lock()) {
                                if (o->isCheating() && o->isEnabled()) o->disable();
                            };
                        };

                        Notification::create("Disabled all cheats", NotificationIcon::Success)->show();
                        if (Menu::get()) menu::open(true);
                    };
                });
        })
        .leak();

    OptionCheatingEvent()
        .listen([](bool cheating) {
            if (mod->getSettingValue<bool>(setting::DynamicSafeMode)) {
                log::warn("Dynamic safe mode is now {}", cheating ? "ON" : "OFF");
                toggleSafeModeHooks(cheating);
            };
        })
        .leak();

    (void)branding::registerBrand(GEODE_MOD_ID, "https://moddev.cheeseworks.gay/cdn/cubic_horriblemenu.webp", branding::Type::URL);
};

// safe mode
class $modify(HMSafeGJGameLevel, GJGameLevel) {
    HORRIBLE_HOOK_INTERNAL(g_safeModeHooks, setting::SafeMode);

    void savePercentage(int, bool, int, int, bool) {
        log::warn("Safe mode is enabled, so progress will not be saved!");
    };
};

// safe mode
class $modify(HMSafePlayLayer, PlayLayer) {
    HORRIBLE_HOOK_INTERNAL(g_safeModeHooks, setting::SafeMode);

    // safe mode prevents level completion
    void levelComplete() {
        log::warn("Safe mode is enabled, so progress will not be saved");

        bool testMode = m_isTestMode;

        m_isTestMode = true;
        PlayLayer::levelComplete();
        m_isTestMode = testMode;
    };
};

static void toggleButton(bool toggle = false, bool editor = false) {
    log::trace("{} floating button", toggle ? "Showing" : "Hiding");

    if (auto fb = MenuButton::get()) {
        auto toggleTo = mod->getSettingValue<bool>(setting::FloatingBtn) && ((editor ? fb->showInEditor() : fb->showInLevel()) || toggle);

        fb->setVisible(toggleTo);
        fb->setTouchEnabled(toggleTo);
    };
};

class $modify(HMFloatBtnLevelEditorLayer, LevelEditorLayer) {
    HORRIBLE_HOOK_INTERNAL(g_floatingBtnHooks, setting::FloatingBtn);

    struct Fields final {
        ~Fields() {
            toggleButton(true, true);
        };
    };

    bool init(GJGameLevel* level, bool noUI) {
        if (!LevelEditorLayer::init(level, noUI)) return false;

        m_fields.self();  // lazy init
        toggleButton(false, true);

        return true;
    };
};

class $modify(HMFloatBtnPauseLayer, PauseLayer) {
    HORRIBLE_HOOK_INTERNAL(g_floatingBtnHooks, setting::FloatingBtn);

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

class $modify(HMFloatBtnPlayLayer, PlayLayer) {
    HORRIBLE_HOOK_INTERNAL(g_floatingBtnHooks, setting::FloatingBtn);

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

    void showEndLayer() {
        toggleButton(true);
        PlayLayer::showEndLayer();
    };

    void onQuit() {
        toggleButton(true);
        PlayLayer::onQuit();
    };
};