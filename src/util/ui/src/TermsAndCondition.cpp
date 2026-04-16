#include <Geode/Geode.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/PlatformToolbox.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include "Geode/ui/Button.hpp"
#include "Geode/ui/MDTextArea.hpp"
#include "Geode/ui/Notification.hpp"
#include "../TermsAndCondition.hpp"
#include <Utils.h>

using namespace geode::prelude;
using namespace horrible::prelude;

TermsAndCondition* TermsAndCondition::create() {
    // make sure that only one instance at a time so it no spammy thing :)))
    static WeakRef<TermsAndCondition> instance;
    if (auto existing = instance.lock()) {
        return nullptr;
    }

    auto ret = new TermsAndCondition();
    if (ret && ret->init()) {
        ret->autorelease();
        instance = ret;
        return ret;
    };

    delete ret;
    return nullptr;
};

TermsAndCondition::~TermsAndCondition() {
    auto playLayer = PlayLayer::get();
    if (!m_accepted && playLayer) {
        playLayer->onQuit();
    }
};

bool TermsAndCondition::init() {
    if (!Popup::init(420.f, 240.f)) return false;

    m_closeBtn->removeFromParent();

    auto playLayer = PlayLayer::get();
    auto gjBase = GJBaseGameLayer::get();

    m_accepted = false;

    if (!playLayer || !gjBase) return false;

    if (!playLayer->canPauseGame()) return false;

    // pause but not actual pausing the game
    // gjBase->m_playbackMode = PlaybackMode::Paused;
    playLayer->pauseSchedulerAndActions();
    gjBase->pauseAudio();
    PlatformToolbox::showCursor();

    setTitle("Terms and Conditions");

    m_tosArea = MDTextArea::create(
        "By using this mod, you agree to the following terms and conditions:\n\n"
        "1. You will not hold the developer liable for any damage caused to your game or account.\n"
        "2. You will not use this mod for cheating or exploiting in multiplayer modes.\n"
        "3. You will not redistribute this mod without permission from the developer.\n"
        "4. You will respect the intellectual property rights of the developer and other modders.\n"
        "5. You will not use this mod for commercial purposes without a license from the developer.\n"
        "6. You will not reverse engineer or decompile this mod without permission from the developer.\n"
        "7. You will not use this mod to create derivative works without permission from the developer.\n"
        "8. You will not use this mod to harass, defame, or discriminate against any individual or group.\n"
        "9. You will not use this mod to distribute malware or harmful software.\n"
        "10. You will not use this mod to violate any applicable laws or regulations.\n\n"
        "By clicking 'Accept', you acknowledge that you have read and agree to these terms and conditions.",
        {380.f, 140.f},
        false);
    m_tosArea->setPosition({m_mainLayer->getContentSize().width / 2.f, m_mainLayer->getContentSize().height / 2.f + 10.f});
    m_mainLayer->addChild(m_tosArea);

    // Add Accept and Decline buttons
    m_acceptButton = geode::Button::createWithLabel("Accept", "goldFont.fnt", [this](auto) {
        onAccept();
    });
    m_declineButton = geode::Button::createWithLabel("Decline", "goldFont.fnt", [this](auto) {
        onDecline();
    });

    m_buttonMenu->addChildAtPosition(m_acceptButton, Anchor::Bottom, {-60.f, 20.f});
    m_buttonMenu->addChildAtPosition(m_declineButton, Anchor::Bottom, {60.f, 20.f});

    return true;
};

void TermsAndCondition::onDecline() {
    auto playLayer = PlayLayer::get();
    Notification::create("You declined the terms and conditions.", NotificationIcon::Error)->show();
};

void TermsAndCondition::onAccept() {
    auto gjBase = GJBaseGameLayer::get();
    auto playLayer = PlayLayer::get();
    // gjBase->m_playbackMode = PlaybackMode::Playing;
    playLayer->resumeSchedulerAndActions();
    gjBase->resumeAudio();
    m_accepted = true;
    onClose(nullptr);
};