#include "../MathQuiz.hpp"

#include <Utils.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

class MathQuiz::Impl final {
public:
    Richard* richard = nullptr;

    int numFirst = 0;
    int numSecond = 0;

    MathOperation operation = MathOperation::Addition;

    int correctAnswer = 0;
    std::vector<int> answers; // 4 answer options

    ProgressBar* timer = nullptr;
    CCMenu* answerMenu = nullptr;
    CCDrawNode* drawNode = nullptr;

    float totalTime = 10.f;
    float timeRemaining = totalTime;
    float timeDt = 0.f;

    bool correct = false;
    Function<void(bool)> callback = nullptr;
};

MathQuiz::MathQuiz() : m_impl(std::make_unique<Impl>()) {};
MathQuiz::~MathQuiz() {};

bool MathQuiz::init() {
    if (!CCBlockLayer::init()) return false;

    setID("math-quiz"_spr);

    m_impl->numFirst = randng::get(10);
    m_impl->numSecond = randng::get(10);

    m_impl->operation = static_cast<MathOperation>(randng::get(3));

    auto const winSize = CCDirector::get()->getWinSize();

    // Create problem label
    std::string problemText;

    // geometry dash
    if (m_impl->operation == MathOperation::Geometry) {
        problemText = "How many sides does this shape have?";

        int sides = randng::get(10, 3);
        m_impl->correctAnswer = sides;

        // draw node and polygon points
        m_impl->drawNode = CCDrawNode::create();
        m_impl->drawNode->setID("polygon");

        auto radius = std::min(winSize.width, winSize.height) / 8.f;
        auto centerX = winSize.width / 2.f - 160.f;
        auto centerY = winSize.height / 2.f;

        std::vector<CCPoint> polyPoints;
        polyPoints.reserve(sides);

        constexpr float PI = 3.14159265358979323846f;
        float theta = (2.f * PI) / sides;

        for (int i = 0; i < sides; ++i) {
            auto angle = theta * i - PI / 2.f;  // start at top

            auto x = radius * cosf(angle);
            auto y = radius * sinf(angle);

            polyPoints.push_back(ccp(x, y));
        };

        // draw the polygon in local coords with drawNode placed at center
        m_impl->drawNode->setPosition({ centerX, centerY });
        m_impl->drawNode->clear();

        ccColor4F const fillColor = { 0.85f, 0.65f, 0.15f, 1.f };
        ccColor4F const borderColor = { 0.05f, 0.05f, 0.05f, 1.f };

        if (m_impl->drawNode->drawPolygon(polyPoints.data(), static_cast<unsigned int>(polyPoints.size()), fillColor, 2.f, borderColor)) addChild(m_impl->drawNode, 99);
    } else {
        std::string operation;
        switch (m_impl->operation) {
        default: [[fallthrough]];

        case MathOperation::Addition:
            operation = "+";
            break;

        case MathOperation::Subtraction:
            operation = "-";
            break;

        case MathOperation::Multiplication:
            operation = "x";
            break;
        };

        switch (m_impl->operation) {
        default: [[fallthrough]];

        case MathOperation::Addition:
            m_impl->correctAnswer = m_impl->numFirst + m_impl->numSecond;
            break;

        case MathOperation::Subtraction:
            m_impl->correctAnswer = m_impl->numFirst - m_impl->numSecond;
            break;

        case MathOperation::Multiplication:
            m_impl->correctAnswer = m_impl->numFirst * m_impl->numSecond;
            break;
        };

        problemText = fmt::format("{} {} {}", m_impl->numFirst, operation, m_impl->numSecond);

        auto equalsLabel = CCLabelBMFont::create("= ?", "goldFont.fnt", getScaledContentWidth() - 1.25f);
        equalsLabel->setID("equals-label");
        equalsLabel->setAlignment(kCCTextAlignmentCenter);
        equalsLabel->setPosition({ winSize.width / 2.f, winSize.height - 100.f });

        addChild(equalsLabel);
    };

    // reuse winSize declared above
    auto problemLabel = CCLabelBMFont::create(problemText.c_str(), "bigFont.fnt", getScaledContentWidth() - 1.25f);
    problemLabel->setID("problem-label");
    problemLabel->setAlignment(kCCTextAlignmentCenter);
    problemLabel->setPosition({ winSize.width / 2.f, winSize.height - 60.f });
    problemLabel->setScale(m_impl->operation == MathOperation::Geometry ? 0.5f : 0.925f);

    addChild(problemLabel, 1);

    // i hope i did this right cheese, u added this progress bar thing
    m_impl->timer = ProgressBar::create();
    m_impl->timer->setID("timer");
    m_impl->timer->setFillColor(colors::yellow);
    m_impl->timer->setStyle(ProgressBarStyle::Solid);
    m_impl->timer->setAnchorPoint({ 0.5, 0.5 });
    m_impl->timer->setPosition({ winSize.width / 2.f, winSize.height - 20.f });

    m_impl->timer->updateProgress(100.f);

    addChild(m_impl->timer, 9);

    m_impl->timeRemaining = m_impl->totalTime = 10.f;
    m_impl->timer->updateProgress(100.f);

    m_impl->answers.push_back(m_impl->correctAnswer);

    // Add 3 wrong answers
    if (m_impl->operation == MathOperation::Geometry) {
        while (m_impl->answers.size() < 4) {
            int wrongAnswer = randng::get(10, 3);
            if (wrongAnswer != m_impl->correctAnswer && !hasAnswer(wrongAnswer)) m_impl->answers.push_back(wrongAnswer);
        };
    } else {
        while (m_impl->answers.size() < 4) {
            int wrongAnswer = m_impl->correctAnswer + randng::get(10, -5);
            if (wrongAnswer != m_impl->correctAnswer && !hasAnswer(wrongAnswer)) m_impl->answers.push_back(wrongAnswer);
        };
    };

    // Shuffle the answers
    utils::random::shuffle(m_impl->answers);

    // richard floating lol
    if (auto richard = Richard::create()) {
        m_impl->richard = richard;
        m_impl->richard->setID("richard");
        m_impl->richard->setAnchorPoint({ 1, 0.5 });
        m_impl->richard->setScale(0.625f);
        m_impl->richard->setPosition({ winSize.width - 36.f, winSize.height / 2.f });

        addChild(m_impl->richard, 99);

        auto moveUp = CCMoveBy::create(1.f, ccp(0, 8.f));
        auto moveDown = CCMoveBy::create(1.f, ccp(0, -8.f));

        auto seq = CCSequence::createWithTwoActions(moveUp, moveDown);

        m_impl->richard->runAction(CCRepeatForever::create(seq));
    };

    auto answerMenuLayout = RowLayout::create()
        ->setGap(2.5f)
        ->setGrowCrossAxis(true);

    m_impl->answerMenu = CCMenu::create();
    m_impl->answerMenu->setID("answer-menu");
    m_impl->answerMenu->setContentSize({ 220.f, 75.f });
    m_impl->answerMenu->setPosition({ winSize.width / 2.f, winSize.height / 2.f - 20.f });
    m_impl->answerMenu->setLayout(answerMenuLayout);

    for (int i = 0; i < 4; i++) {
        auto btnSprite = ButtonSprite::create(
            fmt::format("{}", m_impl->answers[i]).c_str(),
            80.f,
            true,
            "bigFont.fnt",
            "GJ_button_01.png",
            0,
            0.825f
        );

        auto answerBtn = CCMenuItemSpriteExtra::create(
            btnSprite,
            this,
            menu_selector(MathQuiz::onAnswerClicked)
        );
        answerBtn->setID("submit-answer-btn");
        answerBtn->setTag(m_impl->answers[i]);

        m_impl->answerMenu->addChild(answerBtn);
    };

    addChild(m_impl->answerMenu);
    m_impl->answerMenu->updateLayout(true);

    scheduleUpdate();

    // @geode-ignore(unknown-resource)
    playSfx("chest07.ogg");

    return true;
};

void MathQuiz::setCallback(Function<void(bool)> cb) {
    m_impl->callback = std::move(cb);
};

void MathQuiz::setCorrect(bool v) {
    m_impl->correct = v;
    // @geode-ignore(unknown-resource)
    playSfx(v ? "crystal01.ogg" : "jumpscareAudio.mp3");
};

void MathQuiz::onAnswerClicked(CCObject* sender) {
    unscheduleUpdate();

    if (auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(sender)) {
        int selectedAnswer = btn->getTag();
        auto correct = (selectedAnswer == m_impl->correctAnswer);

        if (m_impl->answerMenu) m_impl->answerMenu->removeFromParentAndCleanup(true);
        if (m_impl->drawNode) m_impl->drawNode->removeFromParentAndCleanup(true);

        auto const winSize = CCDirector::get()->getWinSize();

        auto feedbackLabel = CCLabelBMFont::create(correct ? "Correct!" : "Incorrect!", "goldFont.fnt");
        feedbackLabel->setID("feedback-label");
        feedbackLabel->setScale(0.125f);
        feedbackLabel->setAnchorPoint({ 0.5, 0.5 });
        feedbackLabel->setAlignment(kCCTextAlignmentCenter);
        feedbackLabel->setPosition(winSize / 2.f);
        feedbackLabel->setColor(correct ? colors::green : colors::red);

        addChild(feedbackLabel, 9);

        // Small scale animation, delay, then call close
        feedbackLabel->runAction(CCSequence::create(
            CCEaseSineOut::create(CCScaleTo::create(0.125f, 1.25f)),
            CCEaseSineOut::create(CCScaleTo::create(0.0875f, 1.f)),
            CCDelayTime::create(0.75f),
            CCCallFuncN::create(this, callfuncN_selector(MathQuiz::closeAfterFeedback)),
            nullptr
        ));

        setKeypadEnabled(false);
        setCorrect(correct);
    };
};

bool MathQuiz::hasAnswer(int answer) const noexcept {
    for (auto const& a : m_impl->answers) if (a == answer) return true;
    return false;
};

void MathQuiz::keyBackClicked() {
    Notification::create("You can't escape the math quiz...", NotificationIcon::Error, 1.25f)->show();

    setCorrect(false);
    removeMeAndCleanup();
};

void MathQuiz::closeAfterFeedback(CCNode*) {
    if (m_impl->callback) m_impl->callback(m_impl->correct);
    removeMeAndCleanup();
};

void MathQuiz::update(float dt) {
    if (m_impl->timeRemaining <= 0.f) return;
    m_impl->timeRemaining -= dt;

    m_impl->timeDt += dt;
    if (m_impl->timeDt >= 0.5f) {
        // @geode-ignore(unknown-resource)
        playSfx("counter003.ogg");
        m_impl->timeDt = 0.f;
    };

    if (m_impl->timeRemaining < 0.f) m_impl->timeRemaining = 0.f;
    float pct = (m_impl->timeRemaining / m_impl->totalTime) * 100.f;

    if (m_impl->timer) m_impl->timer->updateProgress(pct);

    if (m_impl->timeRemaining <= 0.f) {
        // automatic incorrect
        setCorrect(false);

        // Notification::create("Time's Up!", NotificationIcon::Error, 1.5f)->show();

        auto const winSize = CCDirector::get()->getWinSize();

        if (m_impl->answerMenu) m_impl->answerMenu->removeFromParentAndCleanup(true);

        auto feedbackLabel = CCLabelBMFont::create("Time's Up!", "goldFont.fnt");
        feedbackLabel->setID("feedback-label");
        feedbackLabel->setAnchorPoint({ 0.5, 0.5 });
        feedbackLabel->setAlignment(kCCTextAlignmentCenter);
        feedbackLabel->setPosition(winSize / 2.f);
        feedbackLabel->setScale(0.1f);
        feedbackLabel->setColor(colors::red);

        addChild(feedbackLabel, 999);

        auto seq = CCSequence::create(
            CCScaleTo::create(0.0875f, 1.25f),
            CCScaleTo::create(0.125f, 1.f),
            CCDelayTime::create(0.75f),
            CCCallFuncN::create(this, callfuncN_selector(MathQuiz::closeAfterFeedback)),
            nullptr
        );

        feedbackLabel->runAction(seq);
        unscheduleUpdate();
    };
};

MathQuiz* MathQuiz::create() {
    auto ret = new MathQuiz();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};

bool Richard::init() {
    if (!CCNode::init()) return false;

    auto sprite = CCSprite::createWithSpriteFrameName("diffIcon_02_btn_001.png");
    sprite->setID("richard");
    sprite->setAnchorPoint({ 0.5f, 0.5f });
    sprite->setScale(5.f);

    setContentSize(sprite->getScaledContentSize());

    sprite->setPosition(getScaledContentSize() / 2.f);

    addChild(sprite);

    return true;
};

Richard* Richard::create() {
    auto ret = new Richard();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};