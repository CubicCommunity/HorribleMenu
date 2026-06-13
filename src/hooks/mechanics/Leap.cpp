#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "leap"

static auto const o = Option::create(THIS_ID)
                          ->setName("Charged Leap")
                          ->setDescription("While you're a ground-based game mode, you'll be forced to charge up every jump you make. The higher the charge, the further upwards and forwards you'll be boosted.\n<cl>created by Cheeseworks</c>")
                          ->setCategory(category::mechanics)
                          ->setSillyTier(SillyTier::High)
                          ->setCheating(true)
                          ->autoRegister();

class $modify(LeapGJBaseGameLayer, GJBaseGameLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        float charge = 0.f;
        float speed = 0.f;

        Ref<ProgressBar> chargeMeter = nullptr;
    };

    HORRIBLE_SETUP_INTERFACE_FUNC {
        auto f = m_fields.self();

        if (!on) {
            cue::resetNode(f->chargeMeter);

            unschedule(schedule_selector(LeapGJBaseGameLayer::chargeUp));
            unschedule(schedule_selector(LeapGJBaseGameLayer::decharge));

            f->charge = 0.f;

            m_player1->m_playerSpeed = f->speed;
            m_player2->m_playerSpeed = f->speed;

            return;
        };

        f->speed = m_player1->m_playerSpeed;

        if (auto pl = PlayLayer::get(); pl && !f->chargeMeter) {
            f->chargeMeter = ProgressBar::create();
            f->chargeMeter->setID("charge-meter"_spr);
            f->chargeMeter->setFillColor(colors::red);
            f->chargeMeter->setAnchorPoint(anchor::center);
            f->chargeMeter->setPosition({pl->m_uiLayer->getScaledContentWidth() / 2.f, 25.f});
            f->chargeMeter->setScale(0.625f);

            pl->m_uiLayer->addChild(f->chargeMeter, HIGHEST_Z);

            f->chargeMeter->updateProgress(0.f);
            f->chargeMeter->setVisible(false);
        };
    };

    bool init() {
        if (!GJBaseGameLayer::init()) return false;

        queueInMainThread([self = WeakRef(this)]() {
            if (auto s = self.lock()) s->setupHorribleInterface();
        });

        return true;
    };

    void handleButton(bool down, int button, bool isPlayer1) {
        if (button != 1 || !isGroundMode(isPlayer1 ? m_player1 : m_player2)) return GJBaseGameLayer::handleButton(down, button, isPlayer1);

        auto f = m_fields.self();

        if (down) {
            f->charge = 0.f;

            schedule(schedule_selector(LeapGJBaseGameLayer::chargeUp), 0.1f);
            if (f->chargeMeter) f->chargeMeter->setVisible(true);
        } else {
            unschedule(schedule_selector(LeapGJBaseGameLayer::chargeUp));
            unschedule(schedule_selector(LeapGJBaseGameLayer::decharge));

            m_player1->m_playerSpeed = f->speed;
            m_player2->m_playerSpeed = f->speed;

            if (f->chargeMeter) {
                f->chargeMeter->setVisible(false);
                f->chargeMeter->updateProgress(0.f);
            };

            if (f->charge <= 0.f) return;

            f->speed = m_player1->m_playerSpeed;  // cba to check p2

            auto pct = f->charge / 100.f;

            auto newSpeed = (f->speed * 2.5f) * pct;
            auto boostHeight = 16.25f * pct;

            m_player1->m_playerSpeed = newSpeed;
            m_player2->m_playerSpeed = newSpeed;

            if (onGround(m_player1)) m_player1->boostPlayer(boostHeight * (m_player1->m_isUpsideDown ? -1.f : 1.f));
            if (onGround(m_player2)) m_player2->boostPlayer(boostHeight * (m_player2->m_isUpsideDown ? -1.f : 1.f));

            GJBaseGameLayer::handleButton(true, button, isPlayer1);

            sfx::play(sfx::file::pop);

            schedule(schedule_selector(LeapGJBaseGameLayer::decharge));

            f->charge = 0.f;

            GJBaseGameLayer::handleButton(false, button, isPlayer1);
        };
    };

    void chargeUp(float dt) {
        auto f = m_fields.self();

        if (m_playerDied) {
            f->charge = 0.f;

            if (f->chargeMeter) {
                f->chargeMeter->setVisible(false);
                f->chargeMeter->updateProgress(0.f);
            };

            setOnGround(m_player1, true);
            setOnGround(m_player2, true);

            handleButton(false, 1, true);

            unschedule(schedule_selector(LeapGJBaseGameLayer::chargeUp));
        };

        f->charge += 20.f;

        if (f->chargeMeter) {
            f->chargeMeter->updateProgress(f->charge);
            f->chargeMeter->setFillColor(colors::fadeColor(f->charge));
        };

        if (f->charge >= 100.f) {
            f->charge = 100.f;
            unschedule(schedule_selector(LeapGJBaseGameLayer::chargeUp));
        };
    };

    void decharge(float dt) {
        auto f = m_fields.self();

        if (m_playerDied) {
            f->charge = 0.f;

            m_player1->m_playerSpeed = f->speed;
            m_player2->m_playerSpeed = f->speed;

            if (f->chargeMeter) {
                f->chargeMeter->setVisible(false);
                f->chargeMeter->updateProgress(0.f);
            };

            setOnGround(m_player1, true);
            setOnGround(m_player2, true);

            unschedule(schedule_selector(LeapGJBaseGameLayer::decharge));
        };

        auto speedFt = dt * 1.5f;

        m_player1->m_playerSpeed -= speedFt;
        m_player2->m_playerSpeed -= speedFt;

        if (m_player1->m_playerSpeed <= f->speed) m_player1->m_playerSpeed = f->speed;
        if (m_player2->m_playerSpeed <= f->speed) m_player2->m_playerSpeed = f->speed;

        if ((m_player1->m_playerSpeed == f->speed) && (m_player2->m_playerSpeed == f->speed)) unschedule(schedule_selector(LeapGJBaseGameLayer::decharge));
    };

    bool isGroundMode(PlayerObject* player) const noexcept {
        if (player) return player->m_isRobot || (!player->m_isShip && !player->m_isBall && !player->m_isBird && !player->m_isDart && !player->m_isRobot && !player->m_isSpider && !player->m_isSwing);
        return false;
    };

    bool onGround(PlayerObject* player) const noexcept {
        if (player) return player->m_isOnGround && player->m_isOnGround2 && player->m_isOnGround3 && player->m_isOnGround4;
        return false;
    };

    void setOnGround(PlayerObject* player, bool onGround) {
        player->m_isOnGround = onGround;
        player->m_isOnGround2 = onGround;
        player->m_isOnGround3 = onGround;
        player->m_isOnGround4 = onGround;
    };
};

$on_mod(Loaded) {
    listenForHorribleOptionChanges(
        THIS_ID,
        [](HorribleOptionSave data) {
            if (auto gjbgl = GJBaseGameLayer::get()) modify_cast<LeapGJBaseGameLayer*>(gjbgl)->setupHorribleInterface(data.enabled);
        });
};