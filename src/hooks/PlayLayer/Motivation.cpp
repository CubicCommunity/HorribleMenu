#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static constexpr auto id = "motivation";

inline static Option const o = {
    id,
    "Motivational Quotes",
    "You'll get motivational messages while playing! The motivator isn't in the best mood, though...\n<cl>Credit: Cheeseworks</c>",
    category::obstructive,
    SillyTier::Low,
};
HORRIBLE_REGISTER_OPTION(o);

static constexpr auto s_msgs = std::to_array<const char*>({
    "Surprised you haven't quit already.",
    "OW! I hit my knee on my desk.",
    "mrrp meow :3",
    "No, it wasn't lag, dummy.",
    "OwO what's this?",
    "If you press Alt and F4 together, you can quit faster.",
    "Did you know? The first level was created in 2013!",
    "Keep going! I guess...",
    "ViPRiN wants to know your location.",
    "I love GD Cologne",
    "Did you know? The game was originally called Geometry Jump.",
    "Pro tip: Don't die.",
    "among us",
    "Enable Placebo",
    "You don't HAVE to beat this level today, or tomorrow.",
    "remember to touch grass",
    "Be Smart. Quit.",
    "This is just a game, don't take it too seriously.",
    "hi, i'm dad",
    "hi my name is firee",
    "gay.",
    "ob it",
    "if you can't beat this level you're noob",
    "yr'oue*",
    "The cake is a lie.",
    "Are you a real gamer?",
    "Record your rage, at least. It's funny.",
    "Stop bothering me.",
    "I think you could use an easier level.",
    "Did you know? The creator of Geometry Dash is Robert.",
    "you are **** map",
    "2.1 coming soon",
    "pancakes or waffles?",
    "Watch out for crashes.",
    "Take a deep breath. Shut down your device.",
    "You should try Soggy Mod.",
    "like and subscribe",
    "Is this Clubstep?",
    "Dude, just don't die.",
    "Jump! Jump! Jump!",
    "Don't forget to blink.",
    "it's time to drink water",
    "Remember to stretch your fingers.",
    "activa cam",
    "Don't die there.",
    "Who dies like that?",
    "1984",
    "Crazy? I was crazy once. They locked me in a room.",
    "Why would you jump there?!",
    "This is why we can't have nice things.",
    "Have you tried turning it off and on again?",
    "Maybe if you used Practice Mode...",
    "Did you know? You can change the color of your ship.",
    "Mega Hack v7 is out!",
    "Zoink who? ...nevermind",
    "Look at you! You're still playing! Oof...",
    "Remember to study.",
    "The weather is nice.",
    "No, please don't sing.",
    "i don't like this level",
    "bruh",
    "Still going, huh?",
    "Better luck next time.",
    "Don't give up now! Maybe you'll get 1% further.",
    "Delete Geometry Dash",
    "check steam",
    "How was your year with ChatGPT?",
    "What do you want?",
    "Can you do it? You tell me.",
    "Believe in yourself. I don't, but you should.",
    "You got this. Probably. I'm not sure.",
    "That wasn't a smart decision right there.",
    "How about a career in finance?",
    "Even I couldn't have died there.",
    "You know what to do.",
    "lord help me",
    "This is fine.",
    "Still at it, huh?",
    "Nothing beats a Jet2 holiday!",
    "Beats me...",
    "Have you ever made an omelette?",
    "GO!",
    "Have you made it past 67% yet?",
    "I think you could use some ASMR.",
    "You gotta chill...",
    "you're chopped",
    "and the crowd goes wild..!",
    "Come on, this can't be that hard...",
    "Put on some chill Lo-fi beats!",
    "BORING!",
    "Try Medium graphics... hehe",
});

class $modify(MotivationPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(id);

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        log::debug("Preparing {} motivational messages", s_msgs.size());

        auto delay = randng::get(10.f, 3.f);
        log::debug("Motivational message will show after {} seconds", delay);

        scheduleOnce(schedule_selector(MotivationPlayLayer::showMessage), delay);
    };

    void showMessage(float) {
        if (!m_hasCompletedLevel) {
            auto label = CCLabelBMFont::create(s_msgs[randng::get(s_msgs.size())], "bigFont.fnt", getScaledContentWidth() - 12.5f);
            label->setAnchorPoint({0.5, 0});
            label->setAlignment(kCCTextAlignmentCenter);
            label->setPosition({getScaledContentWidth() / 2.f, -label->getScaledContentHeight()});

            auto action = CCSequence::create(
                CCEaseSineOut::create(CCMoveTo::create(0.375f, {label->getPositionX(), 12.5f})),
                CCDelayTime::create(5.f),
                CCEaseSineIn::create(CCMoveTo::create(0.5f, {label->getPositionX(), -label->getScaledContentHeight()})),
                CCCallFuncN::create(this, callfuncN_selector(MotivationPlayLayer::scheduleNextMessage)),
                nullptr);

            m_uiLayer->addChild(label, 9);

            // @geode-ignore(unknown-resource)
            sfx::play("crystal01.ogg");
            label->runAction(action);
        };
    };

    void scheduleNextMessage(CCNode* sender) {
        if (sender) sender->removeMeAndCleanup();

        auto delay = randng::get(15.f, 5.f);
        log::debug("Motivational message will show again after {} seconds", delay);

        scheduleOnce(schedule_selector(MotivationPlayLayer::showMessage), delay);
    };
};