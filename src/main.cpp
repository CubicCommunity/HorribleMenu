#include <Utils.hpp>

#include <menu/OptionMenuButton.hpp>
#include <menu/SettingV3.hpp>

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/GJGameLevel.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

using namespace horrible::prelude;

$on_game(Loaded) {
    listenForKeybindSettingPresses(
        "key-popup",
        [](Keybind const&, bool down, bool repeat, double) {
            if (down && !repeat) menu::open();
        }
    );

    listenForSettingChanges<bool>(
        "floating-button",
        [](bool value) {
            if (auto fb = OptionMenuButton::get()) fb->setVisible(value);
        }
    );

    listenForSettingChanges<bool>(
        "floating-button-level",
        [](bool value) {
            if (auto fb = OptionMenuButton::get()) fb->setShowInLevel(value);
        }
    );

    listenForSettingChanges<double>(
        "floating-button-scale",
        [](double value) {
            if (auto fb = OptionMenuButton::get()) fb->setScale(static_cast<float>(value));
        }
    );

    listenForSettingChanges<int64_t>(
        "floating-button-opacity",
        [](int64_t value) {
            if (auto fb = OptionMenuButton::get()) fb->setOpacity(value);
        }
    );

    (void)horribleMod->registerCustomSettingType("menu", &HorribleSettingV3::parse);

    if (auto fb = OptionMenuButton::get()) OverlayManager::get()->addChild(fb);
};

class $modify(HIMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        if (auto gm = GameManager::get()) {
            // get and store user current fps
            float currentFPS = gm->m_customFPSTarget;
            (void)horribleMod->setSavedValue<float>("fps", currentFPS);

            log::debug("Store Current FPS: {}", currentFPS);
        };

        return true;
    };
};

class $modify(HIPlayLayer, PlayLayer) {
    struct Fields {
        bool safeMode = horribleMod->getSettingValue<bool>("safe-mode");
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        toggleButton();
    };

    void resume() {
        PlayLayer::resume();
        toggleButton();
    };

    void resumeAndRestart(bool fromStart) {
        PlayLayer::resumeAndRestart(fromStart);
        toggleButton();
    };

    void showEndLayer() {
        PlayLayer::showEndLayer();
        toggleButton(true);
    };

    // safe mode prevents level completion
    void levelComplete() {
        auto f = m_fields.self();

        if (f->safeMode) {
            log::info("Safe mode is enabled");

            bool testMode = m_isTestMode;

            m_isTestMode = f->safeMode;
            PlayLayer::levelComplete();
            m_isTestMode = testMode;
        } else {
            log::warn("Safe mode is disabled");
            PlayLayer::levelComplete();
        };
    };

    void toggleButton(bool toggle = false) {
        if (auto fb = OptionMenuButton::get()) fb->setVisible(horribleMod->getSettingValue<bool>("floating-button") && (fb->showInLevel() || toggle));
    };
};

class $modify(HIPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();
        if (auto fb = OptionMenuButton::get()) fb->setVisible(horribleMod->getSettingValue<bool>("floating-button"));
    };
};

// safe mode
class $modify(HIGJGameLevel, GJGameLevel) {
    struct Fields {
        bool safeMode = horribleMod->getSettingValue<bool>("safe-mode");
    };

    void savePercentage(int percent, bool isPracticeMode, int clicks, int attempts, bool isChkValid) {
        if (m_fields->safeMode) {
            log::warn("Safe mode is enabled, your progress will not be saved!");
        } else {
            GJGameLevel::savePercentage(percent, isPracticeMode, clicks, attempts, isChkValid);
        };
    };
};

// WOMP WOMP BROKEN PLAYTEST
class $modify(HILevelEditorLayer, LevelEditorLayer) {
    void onPlaytest() {
        FLAlertLayer::create(
            "Warning",
            "<cg>Horrible Ideas</c> has somehow <cr>completely broke the Playtest function</c> in the level editor, now preventing the player from playtesting levels... Sorry about that!\n\n<cy>We recommend disabling the mod while working on levels in the meantime.</c>",
            "OK")
            ->show();

        LevelEditorLayer::onPlaytest();
    };
};