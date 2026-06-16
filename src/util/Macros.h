#pragma once

#define HIGHEST_Z cocos2d::CCScene::get()->getHighestChildZ() + 1

#define HORRIBLE_SETUP_INTERFACE_FUNC_NAME setupHorribleInterface

#define HORRIBLE_SETUP_INTERFACE_FUNC void HORRIBLE_SETUP_INTERFACE_FUNC_NAME(bool on = true)

#define HORRIBLE_MODIFY_EVENT_HANDLER(Base, Derived)                                                                           \
    horrible::listenForHorribleOptionChanges(                                                                                  \
        THIS_ID,                                                                                                               \
        [](HorribleOptionSave data) {                                                                                          \
            if (auto b = Base::get()) geode::cast::modify_cast<Derived*>(b)->HORRIBLE_SETUP_INTERFACE_FUNC_NAME(data.enabled); \
        })

#define HORRIBLE_TOGGLE_MODIFY(Base, Derived)         \
    $on_mod(Loaded) {                                 \
        HORRIBLE_MODIFY_EVENT_HANDLER(Base, Derived); \
    }

#define HORRIBLE_JUMPSCARES_GRIEF 129066933
#define HORRIBLE_JUMPSCARES_CONGREG 129066879
#define HORRIBLE_JUMPSCARES_TIDAL 93733469
#define HORRIBLE_JUMPSCARES_TROLL 57436521