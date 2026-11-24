#include <Util.h>

#include <rhatt/prelude.h>

#include <ui/Menu.h>
#include <ui/MenuButton.h>

#include <Geode/Geode.hpp>

// #include <cheeseworks.moddevbranding/include/OptionalAPI.hpp>

using namespace geode::prelude;
using namespace cs::brkd::horrible::prelude;

static bool g_loadedOnce = false;

$on_game(Loaded) {
    OverlayManager::get()->addChild(MenuButton::get());

    listenForKeybindSettingPresses(
        "key-popup",
        [](auto, bool down, bool repeat, auto) {
            if (down && !repeat) menu::open();
        }
    );

    listenForSettingChanges<bool>(
        "floating-btn-level",
        [](bool value) {
            if (auto fb = MenuButton::get()) fb->setShowInLevel(value);
        }
    );

    listenForSettingChanges<bool>(
        "floating-btn-editor",
        [](bool value) {
            if (auto fb = MenuButton::get()) fb->setShowInEditor(value);
        }
    );

    listenForSettingChanges<float>(
        "floating-btn-scale",
        [](float value) {
            if (auto fb = MenuButton::get()) fb->setScale(value);
        }
    );

    listenForSettingChanges<uint8_t>(
        "floating-btn-opacity",
        [](uint8_t value) {
            if (auto fb = MenuButton::get()) fb->setOpacity(value);
        }
    );

    listenForSettingChanges<std::string>(
        "floating-btn-icon",
        [](std::string value) {
            if (auto fb = MenuButton::get()) fb->setButtonIcon(std::move(value));
        }
    );

    listenForSettingChanges<std::string>(
        "theme",
        [](std::string value) {
            if (auto fb = MenuButton::get()) fb->setTheme(std::move(value));
        }
    );

    listenForAllHorribleOptionChanges(
        [](std::string_view id, OptionSave data) {
            log::trace("Global options listener detected {} being {}, {}, {}", id, data.enabled ? "enabled" : "disabled", data.pin ? "pinned" : "unpinned", data.viewed ? "viewed" : "not viewed yet");
        }
    );

    ButtonSettingPressedEventV3(mod, "btn-popup")
        .listen([](std::string_view buttonKey) {
            menu::open();
        })
        .leak();

    ButtonSettingPressedEventV3(mod, "utils")
        .listen([](std::string_view buttonKey) {
            if (buttonKey == "copy-list") {
                std::string list;

                for (auto const& [id, o] : OptionManager::get()->getAllOptions()) {
                    if (o->isEnabled()) list = fmt::format("{}{} | {}\n", list, o->isCheating() ? "!" : " ", id);
                };

                if (list.empty()) {
                    Notification::create("No options enabled", NotificationIcon::Error)->show();
                } else {
                    clipboard::write(list);
                    Notification::create("Copied list to clipboard", NotificationIcon::Success)->show();
                };
            } else if (buttonKey == "disable-cheats") {
                cw::showAlert(
                    "Disable All Cheats",
                    "Are you sure you want to <cr>disable all options marked as cheats</c>?",
                    "Cancel",
                    "Yes",
                    [](auto, bool ok) {
                        if (ok) {
                            for (auto const& [id, o] : OptionManager::get()->getAllOptions()) {
                                if (o->isCheating() && o->isEnabled()) o->disable();
                            };

                            Notification::create("Disabled all cheats", NotificationIcon::Success)->show();
                            if (Menu::get()) menu::open(true);
                        };
                    }
                );
            };
        })
        .leak();

    // (void)branding::registerBrand(GEODE_MOD_ID, "https://moddev.cheeseworks.gay/cdn/cubic_horriblemenu.webp", branding::Type::URL);

    g_loadedOnce = true;
}

$on_game(TexturesLoaded) {
    if (!g_loadedOnce) return;  // trigger the button setup to match graphics quality !!!
    if (auto fb = MenuButton::get()) fb->setTheme(mod->getSettingValue<std::string>("theme"));
}