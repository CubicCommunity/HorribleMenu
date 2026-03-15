#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static constexpr auto id = "sticky";

inline static Option const o = {
    id,
    "Sticky Grounds",
    "When your character lands on an object, it may stay stuck on its surface until you jump again.\n<cl>Credit: Cheeseworks</c>",
    category::misc,
    SillyTier::Medium,
};
HORRIBLE_REGISTER_OPTION(o);

class $modify(StickyPlayerObject, PlayerObject) {
    HORRIBLE_DELEGATE_HOOKS(id);

    struct Fields {
        int chance = options::getChance(id);

        float m_defSpeed = 0.f;

        CCLabelBMFont* m_clickLabel = nullptr;

        bool m_onGround = true;
    };

    bool init(int player, int ship, GJBaseGameLayer* gameLayer, CCLayer* layer, bool playLayer) {
        if (!PlayerObject::init(player, ship, gameLayer, layer, playLayer)) return false;

        auto f = m_fields.self();

        if (auto pl = PlayLayer::get()) {
            f->m_clickLabel = CCLabelBMFont::create("Press again to un-stick!", "bigFont.fnt", pl->getScaledContentWidth() - 12.5f);
            f->m_clickLabel->setID("sticky-alert"_spr);
            f->m_clickLabel->setScale(0.625f);
            f->m_clickLabel->setAlignment(kCCTextAlignmentCenter);
            f->m_clickLabel->setAnchorPoint({0.5, 0});
            f->m_clickLabel->setPosition({pl->getScaledContentWidth() / 2.f, 25.f});
            f->m_clickLabel->setVisible(false);

            auto seq = CCSequence::create(
                CCCallFunc::create(this, callfunc_selector(StickyPlayerObject::stickyCol1)),
                CCDelayTime::create(0.125f),
                CCCallFunc::create(this, callfunc_selector(StickyPlayerObject::stickyCol2)),
                CCDelayTime::create(0.125f),
                nullptr);

            pl->m_uiLayer->addChild(f->m_clickLabel, 9);
            f->m_clickLabel->runAction(CCRepeatForever::create(seq));
        };

        f->m_onGround = onGround();

        return true;
    };

    void stickyCol1() {
        auto f = m_fields.self();

        if (f->m_clickLabel) f->m_clickLabel->setColor(colors::yellow);
    };

    void stickyCol2() {
        auto f = m_fields.self();

        if (f->m_clickLabel) f->m_clickLabel->setColor(colors::white);
    };

    bool onGround() {
        // log::trace("1: {} 2: {} 3: {} 4: {}", m_isOnGround ? "y" : "n", m_isOnGround2 ? "y" : "n", m_isOnGround3 ? "y" : "n", m_isOnGround4 ? "y" : "n");
        return m_isOnGround && m_isOnGround2 && m_isOnGround3 & m_isOnGround4;
    };

    void hitGround(GameObject* object, bool notFlipped) {
        auto f = m_fields.self();

        auto wasOnGround = f->m_onGround;
        PlayerObject::hitGround(object, notFlipped);
        auto nowOnGround = onGround();

        if (m_hasEverJumped) {
            if (!wasOnGround && nowOnGround) {
                if (randng::fast() < f->chance) {
                    f->m_defSpeed = m_playerSpeed;
                    m_playerSpeed = 0.f;
                    if (f->m_clickLabel) f->m_clickLabel->setVisible(true);
                };
            };

            f->m_onGround = nowOnGround;
        };
    };

    bool pushButton(PlayerButton button) {
        if (!PlayerObject::pushButton(button)) return false;

        auto f = m_fields.self();

        if (m_playerSpeed <= 0.f && f->m_onGround) {
            m_playerSpeed = f->m_defSpeed;
            if (f->m_clickLabel) f->m_clickLabel->setVisible(false);
        };

        f->m_onGround = onGround();

        return true;
    };
};