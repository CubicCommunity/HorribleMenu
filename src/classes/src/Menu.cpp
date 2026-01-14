#include "../Menu.hpp"

#include <menu/OptionMenuPopup.hpp>

#include <Utils.hpp>

using namespace horrible::util;

void menu::open() {
    if (auto old = OptionMenuPopup::get()) {
        old->removeMeAndCleanup();
    } else if (auto popup = OptionMenuPopup::create()) {
        popup->show();
    };
};