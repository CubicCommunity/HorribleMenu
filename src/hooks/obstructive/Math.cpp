#include <Util.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "math_quiz"

static auto const o = Option::create(THIS_ID)
                          ->setName("Richard's Math Quiz!")
                          ->setDescription("When playing a level in Practice mode, there's a chance Richard will pop out and give you a quick math quiz. Answer correctly to continue, or restart the level from the beginning.\n<cl>suggested by CyanBoi</c>")
                          ->setCategory(category::obstructive)
                          ->setSillyTier(SillyTier::High)
                          ->autoRegister();

class $modify(MathPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        uint8_t chance = options::getChance(THIS_ID);

        Ref<MathQuiz> currentMath = nullptr;
    };

    HORRIBLE_SETUP_INTERFACE_FUNC {
        if (!on) {
            cue::resetNode(m_fields->currentMath);

            unschedule(schedule_selector(MathPlayLayer::doQuiz));

            return;
        };

        nextQuiz();
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        HORRIBLE_SETUP_INTERFACE_FUNC_NAME();
    };

    void resetLevelFromStart() {
        PlayLayer::resetLevelFromStart();
        cue::resetNode(m_fields->currentMath);
    };

    void levelComplete() {
        PlayLayer::levelComplete();
        cue::resetNode(m_fields->currentMath);
    };

    void togglePracticeMode(bool practiceMode) {
        PlayLayer::togglePracticeMode(practiceMode);

        cue::resetNode(m_fields->currentMath);
        nextQuiz();
    };

    void nextQuiz() {
        log::trace("scheduling math quiz");

        unschedule(schedule_selector(MathPlayLayer::doQuiz));
        if (!m_hasCompletedLevel) scheduleOnce(schedule_selector(MathPlayLayer::doQuiz), rng::get(30.f, 5.f) * rng::chanceToDelayPct(m_fields->chance));
    };

    void doQuiz(float) {
        auto f = m_fields.self();

        if (options::isEnabled(THIS_ID) && !f->currentMath && m_isPracticeMode && !m_hasCompletedLevel && !m_playerDied) {
            log::debug("Showing math quiz");

            if (auto quiz = MathQuiz::create()) {
                // handle correct/wrong answer
                quiz->setCallback([this](bool correct) {
                    if (!correct) resetLevelFromStart();
                    nextQuiz();

                    cursor::hide();

                    cue::resetNode(m_fields->currentMath);
                });

                m_uiLayer->addChild(quiz, HIGHEST_Z);
                f->currentMath = quiz;

                cursor::show();
            };
        };

        queueInMainThread([self = WeakRef(this)]() {
            if (auto s = self.lock()) s->nextQuiz();
        });
    };
};

$on_mod(Loaded) {
    listenForHorribleOptionChanges(
        THIS_ID,
        [](HorribleOptionSave data) {
            if (auto pl = PlayLayer::get()) modify_cast<MathPlayLayer*>(pl)->HORRIBLE_SETUP_INTERFACE_FUNC_NAME(data.enabled);
        });
};

HORRIBLE_TOGGLE_MODIFY(PlayLayer, MathPlayLayer);