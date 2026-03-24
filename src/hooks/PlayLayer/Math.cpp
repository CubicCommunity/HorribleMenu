#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

static constexpr auto id = "math_quiz";

static auto const o = Option::create(id)
                          .setName("Richard's Math Quiz!")
                          .setDescription("When playing a level in Practice mode, there's a chance Richard will pop out and give you a quick math quiz. Answer correctly to continue, or restart the level from the beginning.\n<cl>Credit: CyanBoi</c>")
                          .setCategory(category::obstructive)
                          .setSillyTier(SillyTier::High);
HORRIBLE_REGISTER_OPTION(o);

class $modify(MathPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(id);

    struct Fields {
        unsigned int chance = options::getChance(id);

        MathQuiz* m_currentMath = nullptr;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        nextQuiz();
    };

    void levelComplete() {
        PlayLayer::levelComplete();
        if (auto quiz = WeakRef(m_fields->m_currentMath).lock()) quiz->removeMeAndCleanup();
    };

    void nextQuiz() {
        log::trace("scheduling math quiz");

        auto f = m_fields.self();

        if (!m_hasCompletedLevel) scheduleOnce(schedule_selector(MathPlayLayer::doQuiz), randng::get(30.f, 5.f) * chanceToDelayPct(f->chance));
    };

    void doQuiz(float) {
        if (m_isPracticeMode && !m_hasCompletedLevel && !m_playerDied) {
            log::debug("Showing math quiz");

            if (options::isEnabled(id)) {
                if (auto quiz = MathQuiz::create()) {
                    // handle correct/wrong answer
                    quiz->setCallback([self = WeakRef(this), math = WeakRef(quiz)](bool correct) {
                        if (auto s = self.lock()) {
                            log::debug("math {}", correct ? "succeeded" : "failed");

                            if (!correct) s->resetLevelFromStart();
                            s->nextQuiz();

                            cursor::hide();

                            if (auto quiz = math.lock()) quiz->removeMeAndCleanup();
                        };
                    });

                    m_uiLayer->addChild(quiz, 99);
                    m_fields->m_currentMath = quiz;

                    cursor::show();
                };
            } else {
                queueInMainThread([self = WeakRef(this)]() {
                    if (auto s = self.lock()) s->nextQuiz();
                });
            };
        };
    };
};