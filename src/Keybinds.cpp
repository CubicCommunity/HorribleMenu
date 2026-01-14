#ifndef GEODE_IS_IOS
#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <geode.custom-keybinds/include/Keybinds.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;
using namespace keybinds;

$on_mod(Loaded) {
    if (auto bm = BindManager::get()) {
        bm->registerBindable({
            "popup"_spr,
            "Show Menu",
            "Open the Horrible Ideas option menu.",
            {
                Keybind::create(KEY_Backslash, Modifier::None),
            },
            "Horrible Ideas",
                             });

        new EventListener([=](InvokeBindEvent* event) {
            if (event->isDown()) menu::open();
            return ListenerResult::Propagate;
                          },
                          InvokeBindFilter(nullptr, "popup"_spr));
    } else {
        log::error("Failed to get keybind manager");
    };
};
#endif