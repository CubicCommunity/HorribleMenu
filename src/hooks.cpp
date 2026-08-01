#include <Utils.h>

#include <ranges>

#include <ui/Menu.h>
#include <ui/MenuButton.h>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/GJGameLevel.hpp>
#include <Geode/modify/EndLevelLayer.hpp>

using namespace horrible::prelude;

static std::vector<std::weak_ptr<Hook>> g_safeModeHooks;
static std::vector<std::weak_ptr<Hook>> g_floatingBtnHooks;

namespace hooks {
    static void setup(auto& self, std::vector<std::weak_ptr<Hook>>& hookVector, std::string_view settingID) {
        utils::StringMap<std::shared_ptr<Hook>> const& hooks = self.m_hooks;
        auto enable = Mod::get()->getSettingValue<bool>(settingID);

        for (auto& hook : hooks | std::views::values) {
            hook->setAutoEnable(enable);
            (void)hook->toggle(enable);

            (void)self.setHookPriorityPre(hook->getDisplayName(), Priority::FirstPre);

            hookVector.push_back(hook);
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

    static void toggleSafeModeHooks(bool value) {
        for (auto const& hook : g_safeModeHooks) {
            if (auto h = hook.lock()) {
                log::trace("Toggling safe mode hook '{}' {}...", h->getDisplayName(), str::isOnOff(value));
                (void)h->toggle(value);
            };
        };
    };
};

#define HORRIBLE_HOOK_INTERNAL(hookVector, settingID) \
    static void onModify(auto& self) {                \
        hooks::setup(self, hookVector, settingID);    \
    }

$on_game(Loaded) {
    if (auto om = OptionManager::get()) hooks::toggleSafeModeHooks(om->shouldBeSafeMode());
    listenForSettingChanges<bool>(
        setting::SafeMode,
        [](bool value) {
            if (!mod->getSettingValue<bool>(setting::DynamicSafeMode)) hooks::toggleSafeModeHooks(value);
        });

    listenForSettingChanges<bool>(
        setting::DynamicSafeMode,
        [](bool) {
            if (auto om = OptionManager::get()) hooks::toggleSafeModeHooks(om->shouldBeSafeMode());
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
                    log::trace("Toggling floating button hook '{}' {}...", h->getDisplayName(), str::isOnOff(value));
                    (void)h->toggle(value);
                };
            };
        });

    OptionCheatingEvent()
        .listen([](bool cheating) {
            if (mod->getSettingValue<bool>(setting::DynamicSafeMode)) {
                log::warn("Dynamic safe mode is now {}", str::isOnOff(cheating));
                hooks::toggleSafeModeHooks(cheating);
            };
        })
        .leak();
};

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

class $modify(HMEndLevelLayer, EndLevelLayer) {
    HORRIBLE_HOOK_INTERNAL(g_safeModeHooks, setting::SafeMode);

    void customSetup() {
        EndLevelLayer::customSetup();

        if (Mod::get()->getSettingValue<bool>("safe-mode-indicator")) {
            auto isSpriteSecret = rng::fast() <= 5;
            auto btn = Button::createWithSpriteFrameName(themes::getIconSprite(isSpriteSecret ? themes::icons::TheYellowOne : themes::icons::Default), [](auto) {
                createQuickPopup(
                    "Safe Mode",
                    "<cj>Safe Mode</c> is <cg>enabled</c> and your progress <cr>has not</c> been saved.",
                    "OK",
                    nullptr,
                    nullptr);
            });
            btn->setScale(.4f);
            btn->setID("safe-mode-indicator"_spr);
            btn->setZOrder(2);
            m_mainLayer->addChildAtPosition(btn, Anchor::Center, {165, -105}, false);
        };
    };
};

class $modify(HMFloatBtnLevelEditorLayer, LevelEditorLayer) {
    HORRIBLE_HOOK_INTERNAL(g_floatingBtnHooks, setting::FloatingBtn);

    struct Fields final {
        ~Fields() {
            hooks::toggleButton(true, true);
        };
    };

    bool init(GJGameLevel* level, bool noUI) {
        if (!LevelEditorLayer::init(level, noUI)) return false;

        m_fields.self();  // lazy init
        hooks::toggleButton(false, true);

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
        hooks::toggleButton();
        PlayLayer::setupHasCompleted();
    };

    void resume() {
        hooks::toggleButton();
        PlayLayer::resume();
    };

    void resumeAndRestart(bool fromStart) {
        hooks::toggleButton();
        PlayLayer::resumeAndRestart(fromStart);
    };

    void showEndLayer() {
        hooks::toggleButton(true);
        PlayLayer::showEndLayer();
    };

    void onQuit() {
        hooks::toggleButton(true);
        PlayLayer::onQuit();
    };
};