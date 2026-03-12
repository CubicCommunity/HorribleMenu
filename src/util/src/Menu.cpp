#include "../Menu.hpp"

#include <Utils.hpp>

using namespace horrible::util;

import OptionMenu;

void menu::open() {
    if (auto old = OptionMenu::get()) {
        old->removeMeAndCleanup();
    } else if (auto popup = OptionMenu::create()) {
        popup->show();
    };
};