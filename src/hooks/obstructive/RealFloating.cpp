#include <Util.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "da_real_floating_btn"

static auto const o = Option::create(THIS_ID)
                          ->setName("da real floating button")
                          ->setDescription("You'll have a floating button constantly zooming around your screen during gameplay. If you press it, your character will immediately die.\n<cl>created by Cheeseworks</c>")
                          ->setCategory(category::obstructive)
                          ->setSillyTier(SillyTier::Medium)
                          ->autoRegister();

static constexpr auto g_icons = std::to_array({
    themes::icons::Default,
    themes::icons::Breakeode,
    themes::icons::Classic,
    themes::icons::ColonThree,
    themes::icons::CryingSkull,
    themes::icons::Durr,
    themes::icons::Pride,
    themes::icons::TransPride,
    themes::icons::SapphireSDK,
    themes::icons::SoggyCat,
    themes::icons::SteamHappy,
    themes::icons::TheYellowOne,
});

static constexpr auto g_frames = std::to_array({
    CircleBaseColor::Blue,
    CircleBaseColor::Cyan,
    CircleBaseColor::DarkAqua,
    CircleBaseColor::DarkPurple,
    CircleBaseColor::Gray,
    CircleBaseColor::Green,
    CircleBaseColor::Pink,
    CircleBaseColor::Red,
});

class $modify(RealFloatingPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        Button* theBtn = nullptr;
    };

    HORRIBLE_SETUP_INTERFACE_FUNC {
        auto f = m_fields.self();

        if (!on) return cue::resetNode(f->theBtn);

        nextPosition(f->theBtn);
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        HORRIBLE_SETUP_INTERFACE_FUNC_NAME(true);
    };

    void resetLevel() {
        PlayLayer::resetLevel();
        nextPosition(m_fields->theBtn);
    };

    void destroyPlayer(PlayerObject* player, GameObject* object) {
        PlayLayer::destroyPlayer(player, object);
        if (object == m_anticheatSpike || !m_playerDied) return;

        auto f = m_fields.self();

        if (f->theBtn) f->theBtn->stopAllActions();
    };

    void nextPosition(CCNode* sender) {
        auto f = m_fields.self();

        auto const pos = sender ? sender->getPosition() : getRandomEdge(m_uiLayer->getScaledContentSize(), {});

        cue::resetNode(f->theBtn);
        sender = nullptr;

        f->theBtn = makeButton(false);

        f->theBtn->setPosition(pos);

        m_uiLayer->addChild(f->theBtn, HIGHEST_Z);

        sfx::play(sfx::file::click);

        f->theBtn->runAction(
            CCSequence::createWithTwoActions(
                CCMoveTo::create(rng::get(1.25f, 0.5f), getRandomEdge(m_uiLayer->getScaledContentSize(), f->theBtn->getPosition())),
                CCCallFuncN::create(this, callfuncN_selector(RealFloatingPlayLayer::nextPosition))));
    };

    Button* makeButton(bool autoPos = true) {
        auto theBtn = Button::createWithNode(
            CircleButtonSprite::createWithSpriteFrameName(
                themes::getIconSprite(g_icons[rng::get(g_icons.size() - 1)]),
                0.875f,
                g_frames[rng::get(g_frames.size() - 1)]),
            [this](auto) {
                destroyPlayer(m_player1, nullptr);
            });
        theBtn->setID("press-me-btn"_spr);
        theBtn->setScale(rng::get(2.5f, 1.25f));

        if (autoPos) theBtn->setPosition(getRandomEdge(m_uiLayer->getScaledContentSize(), theBtn->getPosition()));

        return theBtn;
    };

    CCPoint getRandomEdge(CCPoint const& maxSize, CCPoint const& currentPos) const {
        auto x = currentPos.x;
        auto y = currentPos.y;

        if (currentPos.x <= 0.f) {
            x = maxSize.x;
            y = rng::get(maxSize.y);
        } else if (currentPos.x >= maxSize.x) {
            x = 0.f;
            y = rng::get(maxSize.y);
        } else if (currentPos.y <= 0.f) {
            x = rng::get(maxSize.x);
            y = maxSize.y;
        } else if (currentPos.y >= maxSize.y) {
            x = rng::get(maxSize.x);
            y = 0.f;
        } else {
            if (rng::flip()) {
                x = (rng::flip() ? 0.f : maxSize.x);
                y = rng::get(maxSize.y);
            } else {
                x = rng::get(maxSize.x);
                y = (rng::flip() ? 0.f : maxSize.y);
            };
        };

        return {x, y};
    };
};

HORRIBLE_TOGGLE_MODIFY(PlayLayer, RealFloatingPlayLayer);