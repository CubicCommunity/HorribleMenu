#include "../Menu.hpp"

#include <menu/OptionMenu.hpp>

#include <Utils.hpp>

using namespace horrible::util;

void menu::open() {
    if (auto old = OptionMenu::get()) {
        old->removeMeAndCleanup();
    } else if (auto popup = OptionMenu::create()) {
        popup->show();
    };
};