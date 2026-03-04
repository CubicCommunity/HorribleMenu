#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static constexpr auto id = "math_quiz";

inline static Option const o = {
    id,
    "Richard's Math Quiz!",
    "When playing a level in Practice mode, there's a chance Richard will pop out and give you a quick math quiz. Answer correctly to continue, or restart the level from the beginning.\n<cy>Credit: CyanBoi</c>",
    category::obstructive,
    SillyTier::High,
};
HORRIBLE_REGISTER_OPTION(o);

class $modify(MathPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(id);

    struct Fields {
        int chance = options::getChance(id);
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        nextQuiz();
    };

    void nextQuiz() {
        log::debug("scheduling math quiz");

        auto f = m_fields.self();

        if (!m_hasCompletedLevel) scheduleOnce(schedule_selector(MathPlayLayer::doQuiz), randng::get(30.f, 5.f) * chanceToDelayPct(f->chance));
    };

    void doQuiz(float) {
        auto f = m_fields.self();

        if (m_isPracticeMode && !m_hasCompletedLevel) {
            log::info("Showing math quiz");

            if (auto quiz = MathQuiz::create()) {
                // handle correct/wrong answer
                quiz->setCallback([this](bool correct) {
                    log::debug("math {}", correct ? "succeeded" : "failed");
                    if (!correct) resetLevelFromStart();
                    nextQuiz();
                    });

#ifdef GEODE_IS_WINDOWS
                CCEGLView::sharedOpenGLView()->showCursor(true);
#endif
                m_uiLayer->addChild(quiz, 99);
            };
        };
    };
};