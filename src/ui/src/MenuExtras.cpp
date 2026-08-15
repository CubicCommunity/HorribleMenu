#include "../MenuExtras.hpp"

#include <Util.h>

#include <argon/argon.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

$on_game(Loaded) {
    async::spawn(
        argon::startAuth(),
        [](Result<std::string> result) {
            if (result.isErr()) return log::error("Failed to authorize with Argon: {}", std::move(result).unwrapErr());

            if (auto gjam = GJAccountManager::sharedState()) {
                if (auto as = AuthState::get()) as->setAuthInfo(gjam->m_accountID, gjam->m_uID, gjam->m_username, std::move(result).unwrap());
                log::info("Authorized {} ({}) with Argon", gjam->m_username, gjam->m_accountID);
            };
        });
};

MenuSuggest* MenuSuggest::s_inst = nullptr;

bool MenuSuggest::init(ZStringView theme) {
    auto btns = themes::getCircleBaseColor(theme);

    if (!Popup::init({345.f, 225.f}, themes::getBackgroundSprite(theme))) return false;

    setID("suggestions"_spr);
    setTitle("Suggest a Feature!");
    setCloseButtonSpr(themes::createThemeCircleSprite(btns));

    popup::closeBtnID(m_closeBtn);

    auto const mainLayerSize = m_mainLayer->getScaledContentSize();

    m_topicInput = TextInput::create(mainLayerSize.width - 25.f, "What's your idea?");
    m_topicInput->setID("topic-input");
    m_topicInput->setPosition({mainLayerSize.width / 2.f, (mainLayerSize.height / 2.f) + 60.f});

    m_mainLayer->addChild(m_topicInput, 1);

    m_descriptionInput = TextInput::create(mainLayerSize.width - 25.f, "Describe your idea in more detail...", "chatFont.fnt");
    m_descriptionInput->setID("description-input");
    m_descriptionInput->setContentHeight(m_descriptionInput->getScaledContentHeight() * 1.5f);
    m_descriptionInput->setPosition({mainLayerSize.width / 2.f, (mainLayerSize.height / 2.f) + 26.5f});

    if (auto descInputNode = m_descriptionInput->getInputNode()) {
        descInputNode->setContentHeight(m_descriptionInput->getScaledContentHeight());  // bruh
        if (auto bg = m_descriptionInput->getBGSprite()) bg->setContentHeight(m_descriptionInput->getScaledContentHeight() * 2.f);
    };

    m_mainLayer->addChild(m_descriptionInput, 9);

    auto instructions = MDTextArea::create(
        "# BEFORE You Send...\n\n"
        "Here are some general guidelines regarding our etiquette and what to expect from this feature!\n\n"
        "- <cg>Breakeode receives every idea you send</c>. **Refrain** from <cr>spamming them</c>!\n"
        "- Provide <cy>as much detail as you can</c> about your idea to give developers **a clear idea** and therefore <cg>credit you</c>!\n"
        "- **Refrain** from <cy>suggesting existing features from prominent Geode mods</c>, <cr>we want originality</c>!\n"
        "- If your idea <cr>didn't make it to the following feature update</c>, it was **probably omitted**.\n"
        "- You're welcome to **join [Breakeode's Discord server](https://dsc.gg/breakeode)** to <cg>personally bring and check up on your ideas</c>, just <cy>be polite</c> is all!\n"
        "- <cy>Attempting to spam this form or our API server</c> will likely get you <cr>rate-limited or IP-banned</c>.",
        {mainLayerSize.width - 25.f, 92.5f});
    instructions->setID("instructions");
    instructions->setAnchorPoint({0.5, 0});

    m_mainLayer->addChildAtPosition(instructions, Anchor::Bottom, {0.f, 12.5f});

    auto submitBtn = Button::createWithNode(
        ButtonSprite::create(
            "Submit",
            "goldFont.fnt",
            themes::getButtonSquareSprite(theme),
            0.875f),
        [this](auto) {
        });
    submitBtn->setID("submit-idea-btn");
    submitBtn->setScale(0.75f);

    m_mainLayer->addChildAtPosition(submitBtn, Anchor::Bottom);

    auto infoBtn = Button::createWithSpriteFrameName(
        "GJ_infoIcon_001.png",
        [](auto) {
            createQuickPopup(
                "Help",
                "This is the <cg>idea suggestion form</c>. You can use it to <cy>send Breakeode your feature ideas for Horrible Menu</c>. Be sure to <cr>read the guidelines before submitting any ideas</c>.",
                "OK",
                nullptr,
                nullptr);
        });
    infoBtn->setID("info-btn");
    infoBtn->setScale(0.75f);

    m_mainLayer->addChildAtPosition(infoBtn, Anchor::TopRight, {-13.75f, -13.75f});

    return true;
};

void MenuSuggest::onExit() {
    s_inst = nullptr;
    Popup::onExit();
};

MenuSuggest* MenuSuggest::get() noexcept {
    return s_inst;
};

MenuSuggest* MenuSuggest::create(ZStringView theme) {
    auto ret = new MenuSuggest();
    if (ret->init(theme)) {
        ret->autorelease();
        s_inst = ret;
        return ret;
    };

    delete ret;
    return nullptr;
};

void AuthState::setAuthInfo(int accountID, int userID, std::string username, std::string token) {
    m_accountID = accountID;
    m_userID = userID;
    m_username = std::move(username);
    m_token = std::move(token);
};

int AuthState::getAccountID() const noexcept {
    return m_accountID;
};

int AuthState::getUserID() const noexcept {
    return m_userID;
};

ZStringView AuthState::getUsername() const noexcept {
    return m_username;
};

ZStringView AuthState::getToken() const noexcept {
    return m_token;
};

MenuDiscord* MenuDiscord::s_inst = nullptr;

bool MenuDiscord::init(ZStringView theme) {
    auto btns = themes::getCircleBaseColor(theme);

    if (!Popup::init({330.f, 225.f}, themes::getBackgroundSprite(theme))) return false;

    setID("discord"_spr);
    setTitle("Join the Community");
    setCloseButtonSpr(themes::createThemeCircleSprite(btns));

    popup::closeBtnID(m_closeBtn);

    auto infoBtn = Button::createWithSpriteFrameName(
        "GJ_infoIcon_001.png",
        [](auto) {
            createQuickPopup(
                "Help",
                "This is the <cg>Discord community menu</c>. You join <cf>Cubic Studios</c>'s Discord community server to chat with others, or <cc>Breakeode</c>'s Discord server to get help with using <cg>Horrible Menu</c> or suggest ideas.\n\n"
                "You can also <cy>link your Discord account with your Geometry Dash account</c> in this menu, which is required to receive any <cd>Ko-fi support perks</c>.",
                "OK",
                nullptr,
                nullptr);
        });
    infoBtn->setID("info-btn");
    infoBtn->setScale(0.75f);

    m_mainLayer->addChildAtPosition(infoBtn, Anchor::TopRight, {-13.75f, -13.75f});

    return true;
};

void MenuDiscord::onExit() {
    s_inst = nullptr;
    Popup::onExit();
};

MenuDiscord* MenuDiscord::get() noexcept {
    return s_inst;
};

MenuDiscord* MenuDiscord::create(ZStringView theme) {
    auto ret = new MenuDiscord();
    if (ret->init(theme)) {
        ret->autorelease();
        s_inst = ret;
        return ret;
    };

    delete ret;
    return nullptr;
};

MenuKofi* MenuKofi::s_inst = nullptr;

bool MenuKofi::init(ZStringView theme) {
    auto btns = themes::getCircleBaseColor(theme);

    if (!Popup::init({365.f, 240.f}, themes::getBackgroundSprite(theme))) return false;

    setID("kofi"_spr);
    setTitle("Support Breakeode");
    setCloseButtonSpr(themes::createThemeCircleSprite(btns));

    popup::closeBtnID(m_closeBtn);

    auto infoBtn = Button::createWithSpriteFrameName(
        "GJ_infoIcon_001.png",
        [](auto) {
            createQuickPopup(
                "Help",
                "This is the <cg>Ko-fi supporter menu</c>. Here you can find <cp>links to support Breakeode, and the perks for doing so</c>!",
                "OK",
                nullptr,
                nullptr);
        });
    infoBtn->setID("info-btn");
    infoBtn->setScale(0.75f);

    m_mainLayer->addChildAtPosition(infoBtn, Anchor::TopRight, {-13.75f, -13.75f});

    return true;
};

void MenuKofi::onExit() {
    s_inst = nullptr;
    Popup::onExit();
};

MenuKofi* MenuKofi::get() noexcept {
    return s_inst;
};

MenuKofi* MenuKofi::create(ZStringView theme) {
    auto ret = new MenuKofi();
    if (ret->init(theme)) {
        ret->autorelease();
        s_inst = ret;
        return ret;
    };

    delete ret;
    return nullptr;
};