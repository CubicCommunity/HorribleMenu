#include "../Cursor.hpp"

#include <Utils.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

void cursor::show() {
    PlatformToolbox::showCursor();
};

void cursor::hide(bool force) {
    if (auto gm = GameManager::sharedState()) {
        if (force || gm->getGameVariable(GameVar::LockCursor)) PlatformToolbox::hideCursor();
    };
};