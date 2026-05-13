#include <Utils.h>

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

        Ref<MathQuiz> m_currentMath = nullptr;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        nextQuiz();
    };

    void levelComplete() {
        PlayLayer::levelComplete();
        cue::resetNode(m_fields->m_currentMath);
    };

    void nextQuiz() {
        log::trace("scheduling math quiz");
        if (!m_hasCompletedLevel) scheduleOnce(schedule_selector(MathPlayLayer::doQuiz), rng::get(30.f, 5.f) * chanceToDelayPct(m_fields->chance));
    };

    void doQuiz(float) {
        if (m_isPracticeMode && !m_hasCompletedLevel && !m_playerDied) {
            log::debug("Showing math quiz");

            if (options::isEnabled(THIS_ID)) {
                if (auto quiz = MathQuiz::create()) {
                    // handle correct/wrong answer
                    quiz->setCallback([self = WeakRef(this), math = WeakRef(quiz)](bool correct) {
                        if (auto s = self.lock()) {
                            log::debug("math {}", correct ? "succeeded" : "failed");

                            if (!correct) s->resetLevelFromStart();
                            s.take()->nextQuiz();

                            cursor::hide();

                            if (auto quiz = math.lock()) quiz.take()->removeFromParent();
                        };
                    });

                    m_uiLayer->addChild(quiz, 99);
                    m_fields->m_currentMath = quiz;

                    cursor::show();
                };
            } else {
                queueInMainThread([self = WeakRef(this)]() {
                    if (auto s = self.lock()) s.take()->nextQuiz();
                });
            };
        };
    };
};