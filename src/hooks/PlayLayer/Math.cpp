#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static Option const o = {
    "math_quiz",
    "Richard's Math Quiz!",
    "When playing a level in Practice mode, there's a chance Richard will pop out and give you a quick math quiz. Answer correctly to continue, or restart the level from the beginning.\n<cy>Credit: CyanBoi</c>",
    category::obstructive,
    SillyTier::High,
};
HORRIBLE_REGISTER_OPTION(o);

class $modify(MathPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(o.id);

    struct Fields {
        int chance = options::getChance(o.id);

        MathQuiz* m_currentQuiz = nullptr;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        nextQuiz();
    };

    void nextQuiz() {
        log::debug("scheduling math quiz");
        if (!m_hasCompletedLevel) scheduleOnce(schedule_selector(MathPlayLayer::doQuiz), randng::get(30.f, 5.f) * chanceToDelayPct(m_fields->chance));
    };

    void doQuiz(float) {
        if (m_isPracticeMode && !m_hasCompletedLevel) {
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
        };
    };
};