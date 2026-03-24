#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

static constexpr auto id = "friends";

static auto const o = Option::create(id)
                          .setName("Friends")
                          .setDescription("Random friends fly across your screen while you play a level!\n<cl>Credit: Cheeseworks</c>")
                          .setCategory(category::obstructive)
                          .setSillyTier(SillyTier::Medium);
HORRIBLE_REGISTER_OPTION(o);

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
    HORRIBLE_DELEGATE_HOOKS(id);

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        auto delay = randng::get(3.f);
        log::trace("Friend will visit after {} seconds", delay);

        scheduleOnce(schedule_selector(FriendsPlayLayer::showAFriend), delay);
    };

    void showAFriend(float) {
        int rnd = randng::fast();

        float xA = -125.f;                           // starting x pos
        float xB = getScaledContentWidth() + 125.f;  // ending x pos

        if ((static_cast<float>(rnd) / 2) <= 50.0) {
            xA = xB;
            xB = -125.f;
        };  // swap sides

        auto rA = randng::pc();
        auto rB = randng::pc();

        float yA = getScaledContentHeight() * rA;  // starting height pos
        float yB = getScaledContentHeight() * rB;  // ending height pos

        auto friendSpr = CCSprite::createWithSpriteFrameName(s_friends[randng::get(s_friends.size() - 1)]);
        friendSpr->setPosition({xA, yA});
        friendSpr->setScale(1.25 * (rB + rA));
        friendSpr->setRotation(180.f * (yA * yB));  // random rotation

        auto dur = 12.5f * rA;
        auto move = CCMoveTo::create(dur, {xB, yB});
        auto rotate = CCRotateBy::create(dur, 90.f * (rB + rA));  // slight rotation while moving

        auto action = CCSpawn::createWithTwoActions(move, rotate);
        auto finish = CCCallFuncN::create(this, callfuncN_selector(FriendsPlayLayer::cleanupFriend));

        auto friendAction = CCSequence::createWithTwoActions(action, finish);
        auto scheduleAction = CCCallFunc::create(this, callfunc_selector(FriendsPlayLayer::scheduleNextFriend));

        m_uiLayer->addChild(friendSpr, 9);
        friendSpr->runAction(CCSpawn::createWithTwoActions(friendAction, scheduleAction));
    };

    void cleanupFriend(CCNode* sender) {
        if (sender) sender->removeMeAndCleanup();
    };

    void scheduleNextFriend() {
        auto delay = randng::get(5.f);
        log::trace("Friend will visit again after {} seconds", delay);

        scheduleOnce(schedule_selector(FriendsPlayLayer::showAFriend), delay);
    };
};