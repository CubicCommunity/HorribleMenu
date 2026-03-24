#include "../Menu.hpp"

#include <menu/OptionMenu.h>

#include <Utils.h>

using namespace horrible::util;

void menu::open() {
    if (auto old = OptionMenu::get()) {
        old->removeMeAndCleanup();
    } else if (auto popup = OptionMenu::create()) {
        popup->show();
    };
};