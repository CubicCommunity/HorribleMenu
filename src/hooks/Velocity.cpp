#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "velocity"

static auto const o = Option::create(THIS_ID)
                          ->setName("Powering Velocity")
                          ->setDescription("Your movement speed gradually decreases the longer you go without pressing your jump button. Hold it to recover your speed.\n<cl>created by Cheeseworks</c>")
                          ->setCategory(category::misc)
                          ->setSillyTier(SillyTier::High)
                          ->setCheating(true)
                          ->autoRegister();

class $modify(VelocityGJBaseGameLayer, GJBaseGameLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        float speed = 0.f;

        Ref<ProgressBar> speedMeter = nullptr;
    };

    bool init() {
        if (!GJBaseGameLayer::init()) return false;

        // gjbgl doesnt init everything instantly
        queueInMainThread([self = WeakRef(this)]() {
            if (auto s = self.lock()) {
                auto f = s->m_fields.self();

                f->speed = s->m_player1->m_playerSpeed * 2.5f;

                s->m_player1->m_playerSpeed = f->speed / 2.f;

                if (auto pl = PlayLayer::get()) {
                    auto speedMeterLabel = CCLabelBMFont::create("Speed", font::big);
                    speedMeterLabel->setScale(0.375f);
                    speedMeterLabel->setColor(colors::gold);
                    speedMeterLabel->setAnchorPoint(anchor::center);
                    speedMeterLabel->setPosition({pl->m_uiLayer->getScaledContentWidth() / 2.f, 37.5f});

                    pl->m_uiLayer->addChild(speedMeterLabel, HIGHEST_Z);

                    f->speedMeter = ProgressBar::create();
                    f->speedMeter->setID("speed-meter"_spr);
                    f->speedMeter->setFillColor(colors::gold);
                    f->speedMeter->setAnchorPoint(anchor::center);
                    f->speedMeter->setPosition({speedMeterLabel->getPositionX(), speedMeterLabel->getPositionY() - 12.5f});
                    f->speedMeter->setScale(0.875f);

                    pl->m_uiLayer->addChild(f->speedMeter, HIGHEST_Z);

                    f->speedMeter->updateProgress(50.f);
                };
            };
        });

        return true;
    };

    void handleButton(bool down, int button, bool isPlayer1) {
        GJBaseGameLayer::handleButton(down, button, isPlayer1);

        if (button == 1) {
            unschedule(down ? schedule_selector(VelocityGJBaseGameLayer::updateSpeed) : schedule_selector(VelocityGJBaseGameLayer::increaseSpeed));
            schedule(down ? schedule_selector(VelocityGJBaseGameLayer::increaseSpeed) : schedule_selector(VelocityGJBaseGameLayer::updateSpeed));
        };
    };

    void resetPlayer() {
        GJBaseGameLayer::resetPlayer();

        auto f = m_fields.self();

        if (f->speed > 0.f) {
            m_player1->m_playerSpeed = f->speed / 2.f;
            m_player2->m_playerSpeed = f->speed / 2.f;
        };

        if (f->speedMeter) f->speedMeter->updateProgress(50.f);
    };

    void increaseSpeed(float dt) {
        if (m_playerDied) return;

        auto f = m_fields.self();

        auto speedFt = dt * 0.375f;
        if (m_player1->m_playerSpeed < f->speed) m_player1->m_playerSpeed += speedFt;
        if (m_player2->m_playerSpeed < f->speed) m_player2->m_playerSpeed += speedFt;

        if (f->speedMeter) f->speedMeter->updateProgress((m_player1->m_playerSpeed / f->speed) * 100.f);
    };

    void updateSpeed(float dt) {
        if (m_playerDied) return;

        auto f = m_fields.self();

        if (m_player1->m_playerSpeed > f->speed) m_player1->m_playerSpeed = f->speed;
        if (m_player2->m_playerSpeed > f->speed) m_player2->m_playerSpeed = f->speed;

        auto speedFt = dt * 0.125f;
        if (m_player1->m_playerSpeed >= 0.125f) m_player1->m_playerSpeed -= speedFt;
        if (m_player2->m_playerSpeed >= 0.125f) m_player2->m_playerSpeed -= speedFt;

        if (f->speedMeter) f->speedMeter->updateProgress((m_player1->m_playerSpeed / f->speed) * 100.f);
    };
};