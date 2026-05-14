#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "friends"

static auto const o = Option::create(THIS_ID)
                          ->setName("Friends")
                          ->setDescription("Random friends fly across your screen while you play a level!\n<cl>created by Cheeseworks</c>")
                          ->setCategory(category::obstructive)
                          ->setSillyTier(SillyTier::Low)
                          ->autoRegister();

static constexpr auto s_friends = std::to_array<const char*>({
    "diffIcon_00_btn_001.png",
    "diffIcon_01_btn_001.png",
    "diffIcon_02_btn_001.png",
    "diffIcon_03_btn_001.png",
    "diffIcon_04_btn_001.png",
    "diffIcon_05_btn_001.png",
    "diffIcon_06_btn_001.png",
    "diffIcon_07_btn_001.png",
    "diffIcon_08_btn_001.png",
    "diffIcon_09_btn_001.png",
    "diffIcon_10_btn_001.png",
    "diffIcon_auto_btn_001.png",
});

class $modify(FriendsPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        auto delay = rng::get(1.25f);
        log::trace("Friend will visit after {} seconds", delay);

        scheduleOnce(schedule_selector(FriendsPlayLayer::showAFriend), delay);
    };

    void showAFriend(float) {
        auto const uiSize = m_uiLayer->getScaledContentSize();

        auto xA = -125.f;                // starting x pos
        auto xB = uiSize.width + 125.f;  // ending x pos

        if (rng::flip()) {
            xA = xB;
            xB = -125.f;
        };  // swap sides

        auto rA = rng::pc();
        auto rB = rng::pc();

        auto yA = uiSize.height * rA;  // starting height pos
        auto yB = uiSize.height * rB;  // ending height pos

        auto friendSpr = CCSprite::createWithSpriteFrameName(s_friends[rng::get(s_friends.size() - 1)]);
        friendSpr->setPosition({xA, yA});
        friendSpr->setScale(0.875f * (rB + rA));
        friendSpr->setRotation(180.f * (yA * yB));  // random rotation

        auto dur = 12.5f * rA;
        auto move = CCMoveTo::create(dur, {xB, yB});
        auto rotate = CCRotateBy::create(dur, 90.f * (rB + rA));  // slight rotation while moving

        auto action = CCSpawn::createWithTwoActions(move, rotate);
        auto finish = CCCallFuncN::create(this, callfuncN_selector(FriendsPlayLayer::cleanupFriend));

        auto friendAction = CCSequence::createWithTwoActions(action, finish);
        auto scheduleAction = CCCallFunc::create(this, callfunc_selector(FriendsPlayLayer::scheduleNextFriend));

        m_uiLayer->addChild(friendSpr, HIGHEST_Z);
        friendSpr->runAction(CCSpawn::createWithTwoActions(friendAction, scheduleAction));
    };

    void cleanupFriend(CCNode* sender) {
        cue::resetNode(sender);
    };

    void scheduleNextFriend() {
        auto delay = rng::get(0.875f);
        log::trace("Friend will visit again after {} seconds", delay);

        scheduleOnce(schedule_selector(FriendsPlayLayer::showAFriend), delay);
    };
};