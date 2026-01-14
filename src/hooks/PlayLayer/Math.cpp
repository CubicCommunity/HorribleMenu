#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class $modify(MathPlayLayer, PlayLayer) {
    struct Fields {
        bool enabled = options::get("math_quiz");
        int chance = options::getChance("math_quiz");

        MathQuiz* m_currentQuiz = nullptr;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        auto f = m_fields.self();

        this->template addEventListener<OptionEventFilter>(
            [this, f](OptionEvent* ev) {
                unschedule(schedule_selector(MathPlayLayer::doQuiz));

                f->enabled = ev->getToggled();

                if (f->enabled) scheduleOnce(schedule_selector(MathPlayLayer::doQuiz), randng::get(30.f, 5.f) * chanceToDelayPct(m_fields->chance));

                return ListenerResult::Propagate;
            },
            "math_quiz"
        );

        if (f->enabled) nextQuiz();
    };

    void nextQuiz() {
        log::debug("scheduling math quiz");
        if (m_fields->enabled && !m_hasCompletedLevel) scheduleOnce(schedule_selector(MathPlayLayer::doQuiz), randng::get(30.f, 5.f) * chanceToDelayPct(m_fields->chance));
    };

    void doQuiz(float) {
        if (m_fields->enabled && m_isPracticeMode && !m_hasCompletedLevel) {
            log::info("Showing math quiz");

            if (auto quiz = MathQuiz::create()) {
                m_fields->m_currentQuiz = quiz;

                // clear pointer on close / handle correct/wrong answer
                m_fields->m_currentQuiz->setCallback([this](bool correct) {
                    nextQuiz();
                    if (!correct) resetLevelFromStart();

                    if (m_fields->m_currentQuiz) m_fields->m_currentQuiz->removeMeAndCleanup();
                    m_fields->m_currentQuiz = nullptr;
                                                     });

#ifdef GEODE_IS_WINDOWS
                CCEGLView::sharedOpenGLView()->showCursor(true);
#endif
                m_uiLayer->addChild(m_fields->m_currentQuiz, 99);
            };
        } else if (m_fields->enabled) {
            nextQuiz();
        };
    };
};