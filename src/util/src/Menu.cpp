#include "../Menu.hpp"

#include <ui/Menu.h>

#include <Utils.h>

using namespace horrible::util;

void menu::open() {
    if (auto old = Menu::get()) {
        old->removeMeAndCleanup();
    } else if (auto popup = Menu::create()) {
        popup->show();
    };
};