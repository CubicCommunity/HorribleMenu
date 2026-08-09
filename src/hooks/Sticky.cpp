#include <Util.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "sticky"

static auto const o = Option::create(THIS_ID)
                          ->setName("Sticky Grounds")
                          ->setDescription("When your character lands on an object, it may stay stuck on its surface until you jump again.\n<cl>created by Cheeseworks</c>")
                          ->setCategory(category::misc)
                          ->setSillyTier(SillyTier::Medium)
                          ->setCheating(true)
                          ->autoRegister();

class $modify(StickyPlayerObject, PlayerObject) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        uint8_t chance = options::getChance(THIS_ID);

        float defSpeed = 0.f;

        CCLabelBMFont* clickLabel = nullptr;

        bool onGround = true;
    };

    HORRIBLE_SETUP_INTERFACE_FUNC {
        auto f = m_fields.self();

        if (!on) {
            cue::resetNode(f->clickLabel);

            m_playerSpeed = f->defSpeed;

            return;
        };

        if (auto pl = PlayLayer::get(); pl && !f->clickLabel && !m_isSecondPlayer) {
            f->clickLabel = CCLabelBMFont::create("Press again to un-stick!", font::big, pl->getScaledContentWidth() - 12.5f);
            f->clickLabel->setID("sticky-alert"_spr);
            f->clickLabel->setScale(0.625f);
            f->clickLabel->setAlignment(kCCTextAlignmentCenter);
            f->clickLabel->setAnchorPoint({0.5, 0});
            f->clickLabel->setPosition({pl->getScaledContentWidth() / 2.f, 25.f});
            f->clickLabel->setVisible(m_playerSpeed <= 0.f);

            auto seq = CCSequence::create(
                CCCallFuncN::create(this, callfuncN_selector(StickyPlayerObject::stickyCol1)),
                CCDelayTime::create(0.125f),
                CCCallFuncN::create(this, callfuncN_selector(StickyPlayerObject::stickyCol2)),
                CCDelayTime::create(0.125f),
                nullptr);

            pl->m_uiLayer->addChild(f->clickLabel, HIGHEST_Z);
            f->clickLabel->runAction(CCRepeatForever::create(seq));
        };
    };

    bool init(int player, int ship, GJBaseGameLayer* gameLayer, CCLayer* layer, bool playLayer) {
        if (!PlayerObject::init(player, ship, gameLayer, layer, playLayer)) return false;

        if (playLayer) {
            m_fields->onGround = onGround();

            HORRIBLE_SETUP_INTERFACE_FUNC_NAME();
        };

        return true;
    };

    void stickyCol1(CCNode* sender) {
        if (auto label = typeinfo_cast<CCLabelBMFont*>(sender)) label->setColor(colors::yellow);
    };

    void stickyCol2(CCNode* sender) {
        if (auto label = typeinfo_cast<CCLabelBMFont*>(sender)) label->setColor(colors::white);
    };

    bool onGround() {
        log::trace("1: {} 2: {} 3: {} 4: {}", m_isOnGround ? "y" : "n", m_isOnGround2 ? "y" : "n", m_isOnGround3 ? "y" : "n", m_isOnGround4 ? "y" : "n");
        return m_isOnGround && m_isOnGround2 && m_isOnGround3 && m_isOnGround4;
    };

    void hitGround(GameObject* object, bool notFlipped) {
        if (!m_gameLayer) return PlayerObject::hitGround(object, notFlipped);

        auto f = m_fields.self();

        auto wasOnGround = f->onGround;
        PlayerObject::hitGround(object, notFlipped);
        auto nowOnGround = onGround();

        if (m_hasEverJumped) {
            if (nowOnGround && !wasOnGround) {
                if (rng::fast() < f->chance) {
                    f->defSpeed = m_playerSpeed;
                    m_playerSpeed = 0.f;

                    if (f->clickLabel) f->clickLabel->setVisible(true);
                };
            };

            f->onGround = nowOnGround;
        };
    };

    bool pushButton(PlayerButton button) {
        if (!PlayerObject::pushButton(button)) return false;

        auto f = m_fields.self();

        if (m_playerSpeed <= 0.f && f->onGround) {
            m_playerSpeed = f->defSpeed;
            if (f->clickLabel) f->clickLabel->setVisible(false);
        };

        f->onGround = onGround();

        return true;
    };
};

$on_mod(Loaded) {
    listenForHorribleOptionChanges(
        THIS_ID,
        [](HorribleOptionSave data) {
            if (auto pl = PlayLayer::get()) modify_cast<StickyPlayerObject*>(pl->m_player1)->HORRIBLE_SETUP_INTERFACE_FUNC_NAME(data.enabled);
        });
};